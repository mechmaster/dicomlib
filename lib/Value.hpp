#ifndef VALUE_HPP_INCLUDE_GUARD_5790364856093
#define VALUE_HPP_INCLUDE_GUARD_5790364856093
#include "VR.hpp"
#include "boost/any.hpp"
#include "boost/shared_ptr.hpp"


namespace dicom
{

	//!Represents the Value of an attribute in a data set.
	/*!
		See 3.5, section 7.1.
		dicom::Value represents a DataElement, excluding the Tag.

		Data is internally managed using a boost::shared_ptr.  This means
		that copying Value objects is not expensive.  Access to the underlying
		data is only permitted via the const Get function, and you cannot modify
		a Value object once it has been constructed, i.e. it's immutable.  This way
		it's safe to share references to the same underlying data.
	*/
    
	struct Value
	{
    private:
		//!Value Representation, see Part 5 section 6.2
		VR vr_;
    public:
        const VR vr()const
        {
            return vr_;
        }
		/*
			Unfortunately we cant do
			template<typename T,VR vr>
			Value(T& data){...}
			because the language provides no way of implicitly CALLING such a constructor!!
		*/

		//!Constructor
		/*!
			\param vr The value representation of this Value.
			\param data This must be of the type specified by VR, else an exception will be thrown.
		*/
		template<typename T>
		Value(VR vr,const T& data)
			:vr_(vr)
		{
			DynamicVRCheck<T>(vr);
			data_=boost::shared_ptr<boost::any>(new boost::any(data));
		}

		//!Constructor
		/*! This constructor allows a empty data_  object
		*/
		Value(VR vr)
			:vr_(vr)
		{
			data_=boost::shared_ptr<boost::any>(new boost::any());
		}

		//!Query
		/*! empty() query if data_ is NULL.
		*/
		bool empty()const
		{
			bool is_empty = false;
			std::string s;
			UID uid;
			switch(vr_)
			{
				case VR_CS:
				case VR_AE:
				case VR_AS:
				case VR_DA:
				case VR_DS:
				case VR_DT:
				case VR_IS:
				case VR_LO:
				case VR_LT:
				case VR_PN:
				case VR_SH:
				case VR_ST:
				case VR_TM:
				case VR_UT:
					Get(s);
					is_empty=(s.length()==0);
					break;
				case VR_UI:
					Get(uid);
					s=uid.str();
					is_empty=(s.length()==0);
					break;
				default:
					is_empty=data_->empty();
			}
			return is_empty;
		}

		//could also have a Get() parametrized on VR:
		//template<VR vr>
		//void Get()
		//!Accesor function
		/*!
		\param t This will receive a copy of the underlying data. It
			must be of the correct type, else a BadVR exception will be thrown
		*/
		template<typename T>
		void Get(T& t) const
		{
 			DynamicVRCheck<T>(vr_);	//check we have the right value representation.
			t=boost::any_cast<T>(*(data_));
		}

		//!Another Get function
		/*!
			This form accesses the underlying data as a const reference.
			It will be faster than the first form, as no copying is involved, but
			you need to explicitly state the item type within the template
			parameter
		*/

		template<typename T>
		const T& Get() const
		{
			DynamicVRCheck<T>(vr_);
			const boost::any& a = *data_;
			const T* pT=boost::any_cast<T>(&a);//this form returns a pointer, see 'any' documentation.
			return *pT;
		}

		//!right shift operator provided for convenience
		/*!
			using this one can write:
		\code
			Value v=(something);
			int i;
			v >> i;
		\endcode
		*/
		template<typename T>
		void operator >> (T& t) const
		{
			Get(t);
		}

	private:
		//!The data itself, implemented using BOOST utilities.
		boost::shared_ptr<boost::any> data_;

	};
}
#endif //VALUE_HPP_INCLUDE_GUARD_5790364856093
