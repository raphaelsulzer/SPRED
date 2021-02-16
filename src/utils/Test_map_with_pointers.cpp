#include "Map_with_pointers.h"
#include <iostream>
#include <string>
#include <cassert>


struct Name{
	std::string name;

	Name(const std::string& name_):name(name_){}

	~Name(){
		std::cerr<<"Destroy "<<name<<std::endl;
	}
};



using namespace std;

int main(){
	Map_with_pointers<std::string,Name> mymap;

	mymap.add("c++",Name("bof"));

	mymap.add("java",Name("ok"));

	mymap.add("caml",Name("great"));

	bool ret = mymap.add("java",Name("ok2"));

	assert((mymap.size()==3) && !ret);

	for(const auto& pair : mymap){
		cout << pair.first<<"->"<<pair.second->name<<endl;
	}

	ret = mymap.remove("caml");
	assert(ret);

	ret = mymap.remove("caml");
	assert(!ret);


	auto res = mymap.get("java");
	cout << "auto res = mymap.get(java) :"<<res->name<<endl;

	assert((mymap.size()==2));

	for(const auto& pair : mymap){
		cout << pair.first<<"->"<<pair.second->name<<endl;
	}




}
