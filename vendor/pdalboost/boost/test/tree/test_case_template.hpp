//  (C) Copyright Gennadiy Rozental 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: -1 $
//
//  Description : defines template_test_case_gen
// ***************************************************************************

#ifndef BOOST_TEST_TREE_TEST_CASE_TEMPLATE_HPP_091911GER
#define BOOST_TEST_TREE_TEST_CASE_TEMPLATE_HPP_091911GER

// Boost.Test
#include <boost/test/detail/config.hpp>
#include <boost/test/detail/global_typedef.hpp>
#include <boost/test/detail/fwd_decl.hpp>
#include <boost/test/detail/workaround.hpp>

#include <boost/test/utils/class_properties.hpp>

#include <boost/test/tree/observer.hpp>


// Boost
#include <boost/shared_ptr.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/function/function0.hpp>

#ifndef BOOST_NO_RTTI
#include <typeinfo> // for typeid
#else
#include <boost/current_function.hpp>
#endif

// STL
#include <string>   // for std::string
#include <list>     // for std::list

#include <boost/test/detail/suppress_warnings.hpp>


//____________________________________________________________________________//

namespace pdalboost {
namespace unit_test {
namespace ut_detail {

// ************************************************************************** //
// **************          test_case_template_invoker          ************** //
// ************************************************************************** //

template<typename TestCaseTemplate,typename TestType>
class test_case_template_invoker {
public:
    void    operator()()    { TestCaseTemplate::run( (pdalboost::type<TestType>*)0 ); }
};

// ************************************************************************** //
// **************           generate_test_case_4_type          ************** //
// ************************************************************************** //

template<typename Generator,typename TestCaseTemplate>
struct generate_test_case_4_type {
    explicit    generate_test_case_4_type( const_string tc_name, const_string tc_file, std::size_t tc_line, Generator& G )
    : m_test_case_name( tc_name )
    , m_test_case_file( tc_file )
    , m_test_case_line( tc_line )
    , m_holder( G )
    {}

    template<typename TestType>
    void        operator()( mpl::identity<TestType> )
    {
        std::string full_name;
        assign_op( full_name, m_test_case_name, 0 );
        full_name += '<';
#ifndef BOOST_NO_RTTI
         full_name += typeid(TestType).name();
#else
        full_name += BOOST_CURRENT_FUNCTION;
#endif
        if( pdalboost::is_const<TestType>::value )
            full_name += "_const";
        full_name += '>';

        m_holder.m_test_cases.push_back( new test_case( ut_detail::normalize_test_case_name( full_name ),
                                                        m_test_case_file,
                                                        m_test_case_line,
                                                        test_case_template_invoker<TestCaseTemplate,TestType>() ) );
    }

private:
    // Data members
    const_string    m_test_case_name;
    const_string    m_test_case_file;
    std::size_t     m_test_case_line;
    Generator&      m_holder;
};

// ************************************************************************** //
// **************              test_case_template              ************** //
// ************************************************************************** //

template<typename TestCaseTemplate,typename TestTypesList>
class template_test_case_gen : public test_unit_generator {
public:
    // Constructor
    template_test_case_gen( const_string tc_name, const_string tc_file, std::size_t tc_line )
    {
        typedef generate_test_case_4_type<template_test_case_gen<TestCaseTemplate,TestTypesList>,TestCaseTemplate> single_test_gen;

        mpl::for_each<TestTypesList,mpl::make_identity<mpl::_> >( single_test_gen( tc_name, tc_file, tc_line, *this ) );
    }

    virtual test_unit* next() const
    {
        if( m_test_cases.empty() )
            return 0;

        test_unit* res = m_test_cases.front();
        m_test_cases.pop_front();

        return res;
    }

    // Data members
    mutable std::list<test_unit*> m_test_cases;
};

} // namespace ut_detail
} // unit_test
} // namespace pdalboost

#include <boost/test/detail/enable_warnings.hpp>

#endif // BOOST_TEST_TREE_TEST_CASE_TEMPLATE_HPP_091911GER
