#ifndef ARRAY_TEST_SUITE_HPP
#define ARRAY_TEST_SUITE_HPP

#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>
#include <pqxx/pqxx>
#include <cgi/core/containers/Properties.hpp>
#include <sstream>

#include <iostream>

class ArrayTestSuite : public CxxTest::TestSuite
{
public:
	void setUp()
	{
		props.set("host", "lacrosse.cgi.missouri.edu");
		props.set("db", "unit_test");
		props.set("username", "atcprocess");
		props.set("password", "c+eQtTJR9A?tZjM");
		
		
		props.set("schema", "pcontrol");
		std::stringstream conss;
		conss << "host=" << props.get("host") << " dbname=" << props.get("db")
		      << " user=" << props.get("username") << " password="
		      << props.get("password");

		conn = new pqxx::connection(conss.str().c_str());
	}

	void tearDown()
	{
		conn->disconnect();
		delete conn;
	}

#define ARRAY_TEST_BODY(func, exp)					\
		std::stringstream ss;					\
		ss << "select ar from " << func << " as zzz (ar)";	\
		pqxx::work trans(*conn);				\
		pqxx::result result = trans.exec(ss.str().c_str());	\
									\
		std::string ar;						\
		for(pqxx::result::const_iterator it = result.begin();	\
		    it != result.end();					\
		    ++it)						\
		{							\
			(*it)["ar"].to(ar);				\
		}							\
									\
		trans.commit();						\
									\
		TS_ASSERT_EQUALS(ar, exp);				

	
#define ARRAY_INT_1 "ARRAY[[2, 7], [1, 8]]"
#define ARRAY_INT_2 "ARRAY[[2, 8], [5, 4]]"
#define ARRAY_FLOAT_1 "ARRAY[[2.7, 7.45], [1.12, 8.159]]"
#define ARRAY_FLOAT_2 "ARRAY[[2.109, 8.0], [5.5, 4.3]]"
#define SCALAR_INT "159"
#define SCALAR_FLOAT "2.71828"

	///
	/// array_add
	/// 
	void test_ArrayAddInts()
	{
		ARRAY_TEST_BODY("arrayAdd("ARRAY_INT_1","ARRAY_INT_2")",
				"{{4,15},{6,12}}");
	}
	void test_ArrayAddFloats()
	{
		ARRAY_TEST_BODY("arrayAdd("ARRAY_FLOAT_1", "ARRAY_FLOAT_2")",
				"{{4.809,15.45},{6.62,12.459}}");
	}

	///
	/// array subtract
	/// 
	void test_ArraySubtractInts()
	{
		ARRAY_TEST_BODY("arraySubtract("ARRAY_INT_1","ARRAY_INT_2")",
				"{{0,-1},{-4,4}}");
	}
	void test_ArraySubtractFloats()
	{
		ARRAY_TEST_BODY("arraySubtract("ARRAY_FLOAT_1", "ARRAY_FLOAT_2")",
				"{{0.591,-0.55},{-4.38,3.859}}");
	}

	///
	/// array multiply scalar
	///
	void test_ArrayMultiplyScalarIntInt()
	{
		ARRAY_TEST_BODY("arrayMultiplyScalar("ARRAY_INT_1", "SCALAR_INT")",
				"{{-194,-679},{-97,-776}}");
	}
	void test_ArrayMultiplyScalarFloatInt()
	{
		ARRAY_TEST_BODY("arrayMultiplyScalar("ARRAY_FLOAT_1", "SCALAR_INT")",
				"{{429.3,1184.55},{178.08,1297.281}}");
	}
	void test_ArrayMultiplyScalarFloatFloat()
	{
		ARRAY_TEST_BODY("arrayMultiplyScalar("ARRAY_FLOAT_1", "SCALAR_FLOAT")",
				"{{7.339356,20.2511860},{3.0444736,22.17844652}}");
	}
	void test_ArrayMultiplyScalarIntFloat()
	{
		ARRAY_TEST_BODY("arrayMultiplyScalar("ARRAY_INT_1", "SCALAR_FLOAT")",
				"{{6,21},{3,24}}");
	}

	///
	/// array divide scalar
	///
	void test_ArrayDivideScalarIntInt()
	{
		ARRAY_TEST_BODY("arrayDivideScalar("ARRAY_INT_1", "SCALAR_INT")",
				"{{0,0},{0,0}}");
	}
	void test_ArrayDivideScalarFloatInt()
	{
		ARRAY_TEST_BODY("arrayDivideScalar("ARRAY_FLOAT_1", "SCALAR_INT")",
				"{{0.01698113207547169811,0.04685534591194968553},{0.00704402515723270440,0.05131446540880503145}}");
	}
	void test_ArrayDivideScalarFloatFloat()
	{
		ARRAY_TEST_BODY("arrayDivideScalar("ARRAY_FLOAT_1", "SCALAR_FLOAT")",
				"{{0.99327515929190517533,2.7407036802684050},{0.41202525126182733199,3.0015303795046868}}");
	}
	void test_ArrayDivideScalarIntFloat()
	{
		ARRAY_TEST_BODY("arrayDivideScalar("ARRAY_INT_1", "SCALAR_FLOAT")",
				"{{0,2},{0,2}}");
	}

	///
	/// greatest elements
	/// 
	void test_ArrayGreatestElementsInts()
	{
		ARRAY_TEST_BODY("arrayGreatestElements("ARRAY_INT_1","ARRAY_INT_2")",
				"{{2,8},{5,8}}");
	}
	void test_ArrayGreatestElementsFloats()
	{
		ARRAY_TEST_BODY("arrayGreatestElements("ARRAY_FLOAT_1", "ARRAY_FLOAT_2")",
				"{{2.7,8.0},{5.5,8.159}}");
	}

	///
	/// least elements
	///
	void test_ArrayLeastElementsInts()
	{
		ARRAY_TEST_BODY("arrayLeastElements("ARRAY_INT_1","ARRAY_INT_2")",
				"{{2,7},{1,4}}");
	}
	void test_ArrayLeastElementsFloats()
	{
		ARRAY_TEST_BODY("arrayLeastElements("ARRAY_FLOAT_1", "ARRAY_FLOAT_2")",
				"{{2.109,7.45},{1.12,4.3}}");
	}

	///
	/// Misc tests of problems that are possible since I had them
	/// but are not apparent and are untested by the tests above:
	/// 
	void test_Misc()
	{
		std::stringstream ss;					
		ss << "select (ar || ARRAY[[3, 4], [1, 22]]) as res from arrayAdd("ARRAY_INT_1","ARRAY_INT_2") as zzz (ar)";	
		pqxx::work trans(*conn);				
		pqxx::result result = trans.exec(ss.str().c_str());	
									
		std::string cat;
		for(pqxx::result::const_iterator it = result.begin();	
		    it != result.end();
		    ++it)			
		{							
			(*it)["res"].to(cat);
		}							

		ss.str("");
		ss << "select CAST(ar as numeric[]) as res from arrayAdd("ARRAY_INT_1","ARRAY_INT_2") as zzz (ar)";	
		result = trans.exec(ss.str().c_str());	
	
		std::string numCast;
		for(pqxx::result::const_iterator it = result.begin();	
		    it != result.end();
		    ++it)			
		{							
			(*it)["res"].to(numCast);
		}				
									
		trans.commit();					

		TS_ASSERT_EQUALS(cat, "{{4,15},{6,12},{3,4},{1,22}}");
		TS_ASSERT_EQUALS(numCast, "{{4,15},{6,12}}");
	}


protected:
private:
	cgi::core::Properties<std::string> props;
	pqxx::connection* conn;
};

#endif
