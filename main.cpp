#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>

#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "lib/dicomlib.hpp"

//#include <FL/Fl.H>

/*
	modify these constants as appropriate for your environment
*/

//const std::string host = "192.168.3.148";
//const std::string local_AE = "mechgrad";
//const std::string remote_AE = "ORTHANC";
//const short remote_port = 4242;		//standard dicom port

const std::string host = "127.0.0.1";
const std::string local_AE = "mechgrad";
const std::string remote_AE = "Dicoogle";
const short remote_port = 1045;		//standard dicom port

unsigned int count = 0;
std::chrono::duration<double> elapsed_seconds;

std::vector<dicom::DataSet> dicomList;

boost::recursive_mutex lockmutex;

namespace demo
{
	void OpenDataSet(std::string filename)
	{
		dicom::DataSet data;
        
        try
        {
          dicom::Read(filename, data);
        }
        catch(...)
        {
          return;
        }

        ++count;

        std::cout << "File Opened" << std::endl;
		std::cout << data << std::endl;


		std::string PatientID;
        std::string PatientBD;
        std::string SeriesNO;
        std::string Modality;
        std::string PatientName;

		data(dicom::TAG_PAT_ID) >> PatientID;
        data(dicom::TAG_PAT_BIRTH_DATE) >> PatientBD;
        data(dicom::TAG_PAT_NAME) >> PatientName;
        data(dicom::TAG_SERIES_NO) >> SeriesNO;
        data(dicom::TAG_MODALITY) >> Modality;

		std::cout << "Patient ID is " << PatientID << std::endl;
        std::cout << "Patient Birth Date is " << PatientBD << std::endl;
        std::cout << "Patient name is " << PatientName << std::endl;
        std::cout << "Series NO = " << SeriesNO << std::endl;
        std::cout << "Modality = " << Modality << std::endl;

		std::cout << "Pixel data is " <<  data(dicom::TAG_PIXEL_DATA) << std::endl;

		
		//We can access underlying pixel data as follows:
		std::vector<unsigned short> PixelData;

		data(dicom::TAG_PIXEL_DATA) >> PixelData;
        
        {
          boost::unique_lock<boost::recursive_mutex> lock(lockmutex);
          dicomList.push_back(data);
        }
	}

	void ConnectToRemoteServer()
	{
		dicom::PresentationContexts presentation_contexts;
		presentation_contexts.Add(dicom::MAMMO_PRES_IMAGE_STORAGE_SOP_CLASS);
		presentation_contexts.Add(dicom::MAMMO_PROC_IMAGE_STORAGE_SOP_CLASS);
		//add further presentation contexts as appropriate...
		dicom::ClientConnection connection(host,remote_port,local_AE,remote_AE,presentation_contexts);
	}

	void PrintResult(const dicom::DataSet& data)
	{
		std::cout << "Query result:" << std::endl << data << std::endl;
	}
	void QueryRemoteServer()
	{
		dicom::PresentationContexts presentation_contexts;
		presentation_contexts.Add(dicom::STUDY_ROOT_QR_FIND_SOP_CLASS);
	
		dicom::ClientConnection connection(host,remote_port,local_AE,remote_AE,presentation_contexts);
		
		
		//build a query to search for all studies on patients names beginning with "A"
		dicom::DataSet query;
		query.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string("STUDY"));
		query.Put<dicom::VR_UI>(dicom::TAG_STUDY_INST_UID,dicom::UID("*"));
		query.Put<dicom::VR_PN>(dicom::TAG_PAT_NAME,std::string("*"));
		
		//execute the query
		std::vector<dicom::DataSet> result = connection.Find(query,dicom::QueryRetrieve::STUDY_ROOT);

		//print the results
		std::for_each(result.begin(),result.end(),PrintResult);
	}

	/*!
		Instruct the pacs server to send the set of images belonging to
        a given study to another DICOM application entity
	*/
	bool DoCMove()
	{
		dicom::PresentationContexts presentation_contexts;
		presentation_contexts.Add(dicom::STUDY_ROOT_QR_MOVE_SOP_CLASS);
		//presentation_contexts.Add(dicom::PATIENT_ROOT_QR_MOVE_SOP_CLASS);

		dicom::ClientConnection connection(host,remote_port,local_AE,remote_AE,presentation_contexts);
		
        static unsigned int count = 0;
        static std::vector<std::string> UID_list = boost::assign::list_of /*("1.2.392.200036.9116.2.6.1.48.1214242831.1408519596.617924")*/
        ("1.2.392.200036.9116.2.6.1.48.1214242831.1408519682.959652")
        ("1.2.392.200036.9116.2.6.1.48.1214242831.1408519696.253333")
        ("1.2.392.200036.9116.2.6.1.48.1214242831.1408520084.921691")
        /*("1.2.392.200036.9116.2.6.1.48.1214242831.1408520181.188813")*/;
        
		//make CMove request
		dicom::DataSet request;
		request.Put<dicom::VR_CS>(dicom::TAG_QR_LEVEL,std::string("SERIES"));
        
        ++count;
        if (count >= UID_list.size())
        {
          count = 0;
        }
		request.Put<dicom::VR_UI>(dicom::TAG_SERIES_INST_UID,dicom::UID(UID_list[count]));
        
        std::string  serverWorkingDirectory("/home/chif/.local/share/SamSMU/pacs/temp/");
        boost::filesystem::path path(serverWorkingDirectory);
        if (boost::filesystem::exists(path)) {
          std::vector<std::string> deletePathList;

          boost::filesystem::recursive_directory_iterator directoryIterator(serverWorkingDirectory), endIterator;
          while (directoryIterator != endIterator) {
            boost::filesystem::path path = directoryIterator->path();

            deletePathList.push_back(path.string());

            ++directoryIterator;
          }

          for (unsigned int i = 0; i < deletePathList.size(); ++i) {
            boost::filesystem::remove_all(deletePathList[i]);
          }
        }
        else {
          std::cout << boost::str(boost::format("Fatal error! path=%s not exists.") % serverWorkingDirectory) << std::endl;

          return false;
        }

        try
        {
          //request that a set of images be sent.
          dicom::DataSet result=connection.Move("MECHGRAD", request, dicom::QueryRetrieve::STUDY_ROOT);
          std::cout << result << std::endl;
        }
        catch(std::exception& e)
        {
          std::string msg = e.what();
          std::cout << msg << std::endl;
        }

        if (boost::filesystem::exists(path)) {
          int cnt = std::count_if(
            boost::filesystem::directory_iterator(serverWorkingDirectory),
            boost::filesystem::directory_iterator(),
            bind(static_cast<bool(*)(const boost::filesystem::path&)>(boost::filesystem::is_regular_file), 
              bind(&boost::filesystem::directory_entry::path, _1)));
          
          std::cout << "File count = " << cnt << std::endl;
          
          return cnt;
        }
        
        return true;
	}
}//namespace demo

void threadFunc(const std::string dicomDir)
{
  boost::filesystem::recursive_directory_iterator dir(dicomDir), end;
  while (dir != end)
  {
    boost::filesystem::path path = dir->path();
    
    std::cout << "path = " << path.string() << std::endl;
    
    if (boost::filesystem::is_regular_file(path))
    {
      try
      {
        demo::OpenDataSet(dir->path().string());
      }
      catch (...)
      {
      }
    }
    
    ++dir;
  }
}

void loadDicomDirectory(const std::string& dicomDir)
{
  boost::filesystem::directory_iterator dir(dicomDir), end;
  
  while (dir != end)
  {
    boost::filesystem::path path = dir->path();
    std::cout << "path = " << path.string() << std::endl;
    
    if (boost::filesystem::is_directory(path))
    {
      boost::thread thread = boost::thread(boost::bind(&threadFunc, path.string()));
    }

    ++dir;
  }
}

int main(int, char**)
{
  //Fl::scheme("gtk+");
  //Fl::lock();
  //Fl::run();
	//const std::string dicomDir="/home/chif/Patients/0295/Berezhnoi_PV_331416/";			//modify this as appropriate

    //const std::string dicomDir="/home/chif/Patients/0026_STRUEVA/dicom/S60_anon/";
  const std::string dicomDir="/home/chif/Patients/";
  
    std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
    startTime = std::chrono::system_clock::now();
    
	try
	{

        std::cout << "*****************************" << std::endl 
        	      << "* dicomlib demo application *" << std::endl
        	      << "*****************************" << std::endl;

        std::cout << std::endl << "Attempting to open a data set from " << dicomDir << std::endl;
        
        /*boost::filesystem::recursive_directory_iterator dir(dicomDir), end;
        
        while (dir != end)
        {
          boost::filesystem::path path = dir->path();
          
          std::cout << "path = " << path.string() << std::endl;
          
          if (boost::filesystem::is_regular_file(path))
          {
            try
            {
              demo::OpenDataSet(dir->path().string());
            }
            catch (...)
            {
            }
          }
          
          ++dir;
        }*/
        
        loadDicomDirectory(dicomDir);

        std::cout << std::endl << "Attempting to connect to the remote server at " << host << ":" << remote_port << std::endl;
		//demo::ConnectToRemoteServer();

        std::cout <<  std::endl << "Attempting to query the remote server..." << std::endl;
		//demo::QueryRemoteServer();
      
        /*for (unsigned int i = 0; i < 100; ++i)
        {
          std::cout <<  std::endl << "Attempting to move DICOM..." << std::endl;
          bool res = demo::DoCMove();
          
          if (!res)
          {
            std::cout << "Test failed" << std::endl;
            break;
          }
        }*/
	}
	catch(std::exception& e)
	{
      std::string msg = e.what();
      std::cout << msg << std::endl;
	}
	
	while (true)
    {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    //std::cout << "file count = " << count << std::endl;
    
    //endTime = std::chrono::system_clock::now();
    //elapsed_seconds = endTime - startTime;
    
    //std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;
  
	return 0;
}
