/*
 * Triangle.h
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <cassert>
#include <set>

namespace Reorient{

struct Triangle{
private:
	std::vector<unsigned> vertices_;
public:
	typedef std::pair<unsigned,unsigned> Edge;

	Triangle(){}

	Triangle(unsigned v0,unsigned v1,unsigned v2){
		vertices_.push_back(v0);
		vertices_.push_back(v1);
		vertices_.push_back(v2);
	}

	unsigned v0() const{
		return vertices_[0];
	}

	unsigned v1() const{
		return vertices_[1];
	}

	unsigned v2() const{
		return vertices_[2];
	}

	std::vector<unsigned>::const_iterator begin() const{
		return vertices_.begin();
	}

	std::vector<unsigned>::const_iterator end() const{
		return vertices_.end();
	}

	/**
	 * changes the orientation of the triangle
	 */
	void flip(){
		assert(vertices_.size()==3);
		std::swap(vertices_[0],vertices_[1]);
	}

	//prerequirement : t1 and t2 share an edge
	friend bool same_orientation(const Triangle & t1,const Triangle & t2){
		std::pair< std::pair<int,int>,std::pair<int,int> > common_ind(common_indices(t1,t2));
		std::pair<int,int> p1 = common_ind.first;
		std::pair<int,int> p2 = common_ind.second;

		for(int i =0 ; i<3; i++){
			if(p1==p2) return true;
			p1.first = (p1.first+1)%3;
			p1.second = (p1.second+1)%3;
		}
		return false;
	}

	//prerequirement : t1 and t2 share an edge
	friend Edge common_edge(const Triangle & t1,const Triangle & t2){
		std::set<unsigned> common_elts;
		common_elts.insert(t1.begin(),t1.end());
		common_elts.insert(t2.begin(),t2.end());
		assert(common_elts.size()>=2);
		auto it = common_elts.begin();
		auto v0 = *it;
		auto v1 = *(++it);
		return Edge(v0,v1);
	}

	friend std::ostream& operator<<(std::ostream& stream,const Triangle& t){
		stream<<"3 "<<t.v0()<<" "<<t.v1()<<" "<<t.v2();
		return stream;
	}

	unsigned operator[](unsigned i) const{
		return vertices_[i];
	}

private:
	//prerequirement : t1 and t2 share an edge uv
	// the result is then a pair of pair ((u1,v1),(u2,v2)) where
	// u1 v1 are indices of u and v in t1 and same for t2.
	friend std::pair< std::pair<int,int>,std::pair<int,int> > common_indices(const Triangle & t1,const Triangle & t2){
		std::pair< std::pair<int,int>,std::pair<int,int> > res;
		bool find_first = false;
		for(int i =0; i<3; ++i){
			for(int j =0;j<3; ++j){
				if(t1[i]==t2[j]){
					if(!find_first){
						find_first = true;
						res.first = std::make_pair(i,j);
					}
					else{
						res.second = std::make_pair(i,j);
						return res;
					}
				}
			}
		}

		std::cerr <<"t1:"<<t1<<std::endl;
		std::cerr <<"t2:"<<t2<<std::endl;
		assert(false);
		return res;
	}
};

}

#endif /* TRIANGLE_H_ */
