//what you need to include
#include "reflection.hpp"


//namespace usage
using namespace agm::reflection;


//example base class
class Base {
public:
    //needed so as that the class gets reflection capabilities
    CLASS(Base, NullClass);

    //a reflected property
    PROPERTY(int, length);

    //a reflected method
    METHOD(public, bool, processLength, (int l));

private:
    //a reflected field
    FIELD(private, int, m_length);

    //property getter
    int get_length() const {
        return m_length;
    }

    //property setter
    void set_length(int l) {
        m_length = l;
    }
};


//a reflected method implementation
bool Base::processLength(int l)
{
    return l == m_length;
}


//a derived class
class Derived : public Base {
public:
    //derived reflected class
    CLASS(Derived, Base);
};


//for the demo
#include <iostream>
using namespace std;


int main()
{
    //a class instance
    Derived derived;

    //get the class of the Derived class
    const Class &derived_class = derived.getClass();

    //print the class name
    cout << derived_class.getName() << endl;

    //print the the m_length field
    const Field &length_field = derived_class.getField("m_length");
    cout << length_field.getType() << " " 
         << length_field.getName() << endl;

    //print the the length property
    const Property &length_prop = derived_class.getProperty("length");
    cout << length_prop.getType() << " " 
         << length_prop.getName() << endl;

    //print the 'processLength()' method
    const Method &process_length_method = 
                 derived_class.getMethod("processLength");
    cout << process_length_method.getType() << " "
         << process_length_method.getName()
         << process_length_method.getArgs()
         << endl;

    //set the length property
    cout << "using length property" << endl;
    length_prop.set(&derived, 10);
    int i;
    length_prop.get(i, &derived);
    cout << "length = " << i << endl;

    //calling the length method
    cout << "calling bool Base::processLength(int)" << endl;
    bool b;
    process_length_method.invoke(b, (Base *)&derived, 10);
    cout << "processLength=" << b << endl;

    return 0;
}