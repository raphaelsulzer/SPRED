/*
 * Debug_msg.h
 *
 *  Created on: Nov 21, 2014
 *      Author: dsalinas
 */

#ifndef DEBUG_MSG_H_
#define DEBUG_MSG_H_


#define DEBUG_VERBOSE
#ifdef DEBUG_VERBOSE
#define RDBG(a) std::cerr << "DBG: " << (a)<<std::endl
#define RDBGMSG(a,b) std::cerr << "DBG: " << a<<b<<std::endl
#define RDBGVALUE(a) std::cerr << "DBG: " <<  #a << ": " << a<<std::endl
#define RDBGCONT(a) std::cerr << "DBG: container "<< #a<<" -> "; for(auto x:a) std::cerr<< x << ","; std::cerr<<std::endl
#else
//#define DBG(a) a
//#define DBGMSG(a,b) b
//#define DBGVALUE(a) a
//#define DBGCONT(a) a
#define RDBG(a)
#define RDBGMSG(a,b)
#define RDBGVALUE(a)
#define RDBGCONT(a)
#endif



#endif /* DEBUG_MSG_H_ */
