/* Copyright 2017-2018 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#ifndef PYGMO_ISLAND_HPP
#define PYGMO_ISLAND_HPP

#include <pygmo/python_includes.hpp>

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/python/object.hpp>
#include <boost/python/object/pickle_support.hpp>
#include <boost/python/tuple.hpp>

#include <pagmo/island.hpp>
#include <pagmo/s11n.hpp>

#include <pygmo/common_base.hpp>
#include <pygmo/object_serialization.hpp>

namespace pagmo
{

namespace detail
{

namespace bp = boost::python;

// Disable the static UDI checks for bp::object.
template <>
struct disable_udi_checks<bp::object> : std::true_type {
};

template <>
struct isl_inner<bp::object> final : isl_inner_base, pygmo::common_base {
    // Just need the def ctor, delete everything else.
    isl_inner() = default;
    isl_inner(const isl_inner &) = delete;
    isl_inner(isl_inner &&) = delete;
    isl_inner &operator=(const isl_inner &) = delete;
    isl_inner &operator=(isl_inner &&) = delete;
    explicit isl_inner(const bp::object &);
    virtual std::unique_ptr<isl_inner_base> clone() const override final;
    [[noreturn]] static void handle_thread_py_exception(const std::string &);
    // Mandatory methods.
    virtual void run_evolve(island &) const override final;
    // Optional methods.
    virtual std::string get_name() const override final;
    virtual std::string get_extra_info() const override final;
    template <typename Archive>
    void save(Archive &ar, unsigned) const
    {
        ar << boost::serialization::base_object<isl_inner_base>(*this);
        ar << pygmo::object_to_vchar(m_value);
    }
    template <typename Archive>
    void load(Archive &ar, unsigned)
    {
        ar >> boost::serialization::base_object<isl_inner_base>(*this);
        std::vector<char> v;
        ar >> v;
        m_value = pygmo::vchar_to_object(v);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    bp::object m_value;
};

} // namespace detail

} // namespace pagmo

// Register the isl_inner specialisation for bp::object.
PAGMO_S11N_ISLAND_EXPORT_KEY(boost::python::object)

namespace pygmo
{

namespace bp = boost::python;

// Serialization support for the island class.
struct island_pickle_suite : bp::pickle_suite {
    static bp::tuple getstate(const pagmo::island &);
    static void setstate(pagmo::island &, const bp::tuple &);
};

} // namespace pygmo

#endif
