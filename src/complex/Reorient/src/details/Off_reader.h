/*
 * Off_reader.h
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#ifndef OFF_READER_H_
#define OFF_READER_H_

namespace Reorient{


	/**
	 * read an off preambule and numbers in the second line.
	 * An off file must have its first/snd line in this format :
	 * OFF
	 * num_vert num_faces num_edges
	 *
	 * A noff file must have its first/snd line in this format :
	 * nOFF
	 * dim num_vert num_faces num_edges
	 *
	 * The number of edges num_edges is optional and can be left to zero.
	 */
	class Off_reader{
	public:
		Off_reader(std::ifstream& stream) :stream_(stream){}

		~Off_reader(){
			stream_.close();
		}

		/**
		 * read an off file and calls the following methods :
		 * void point(const std::vector<double>& point);
		 * void maximal_face(const std::list<int>& face);
		 * of the visitor when reading a point or a maximal face.
		 */
		template<typename OffVisitor>
		bool read(OffVisitor& off_visitor){
			bool success_read_off_preambule = read_off_preambule();
			if (!success_read_off_preambule) {
				std::cerr << "could not read off preambule\n";
				return false;
			}

			bool success_read_off_points = read_off_points(off_visitor);
			if (!success_read_off_preambule) {
				std::cerr << "could not read off points\n";
				return false;
			}

			bool success_read_off_faces = read_off_faces(off_visitor);
			if (!success_read_off_preambule) {
				std::cerr << "could not read off faces\n";
				return false;
			}

			off_visitor.done();
			return success_read_off_preambule && success_read_off_points && success_read_off_faces;
		}

	private:
		std::ifstream& stream_;

		struct Off_info{
			int dim;
			int num_vertices;
			int num_edges;
			int num_faces;
		};

		Off_info off_info_;

		bool read_off_preambule(){
			std::string line;
			if (!goto_next_uncomment_line(line)) return false;

			bool is_off_file = (line.find("OFF") != std::string::npos);
			bool is_noff_file = (line.find("nOFF") != std::string::npos);

			if (!is_off_file && !is_noff_file) return false;

			if (!goto_next_uncomment_line(line)) return false;
			std::istringstream iss(line);
			if (is_off_file){
				off_info_.dim = 3;
				if (iss >> off_info_.num_vertices >> off_info_.num_faces >> off_info_.num_edges) return true;
				else return false;
				//dont blame me, blame visual 
				//return iss >> off_info_.num_vertices >> off_info_.num_faces >> off_info_.num_edges;
			}
			else
			if (iss >> off_info_.dim >> off_info_.num_vertices >> off_info_.num_faces >> off_info_.num_edges)
				return true;
			else
				return false;
		}

		bool goto_next_uncomment_line(std::string& uncomment_line){
			uncomment_line.clear();
			do
			std::getline(stream_, uncomment_line);
			while (uncomment_line[0] == '%');
			return (uncomment_line.size() > 0 && uncomment_line[0] != '%');
		}


		template<typename OffVisitor>
		bool read_off_points(OffVisitor& visitor){
			int num_vertices_to_read = off_info_.num_vertices;
			while (num_vertices_to_read--){
				std::string line;
				if (!goto_next_uncomment_line(line)) return false;
				std::vector<double> point;
				std::istringstream iss(line);
				point.assign(std::istream_iterator<double>(iss), std::istream_iterator<double>());
				if (point.size() != off_info_.dim) return false;
				visitor.point(point);
			}
			return true;
		}

		template<typename OffVisitor>
		bool read_off_faces(OffVisitor& visitor){
			std::string line;
			while (goto_next_uncomment_line(line)){
				std::istringstream iss(line);
				int num_face_vertices;
				iss >> num_face_vertices;
				std::vector<int> face;
				face.assign(std::istream_iterator<int>(iss), std::istream_iterator<int>());
				if (!face.size() == off_info_.num_vertices) return false;
				visitor.maximal_face(face);
			}
			return true;
		}
	};

}

#endif /* OFF_READER_H_ */
