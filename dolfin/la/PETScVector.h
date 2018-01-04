// Copyright (C) 2004-2016 Johan Hoffman, Johan Jansson, Anders Logg
// and Garth N. Wells
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Garth N. Wells, 2005-2010.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
// Modified by Martin Alnæs, 2008.
// Modified by Fredrik Valdmanis, 2011.

#ifndef __DOLFIN_PETSC_VECTOR_H
#define __DOLFIN_PETSC_VECTOR_H

#ifdef HAS_PETSC

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <petscsys.h>
#include <petscvec.h>

#include <dolfin/log/log.h>
#include <dolfin/common/ArrayView.h>
#include <dolfin/common/types.h>
#include "PETScObject.h"
#include "IndexMap.h"

namespace dolfin
{

  class SparsityPattern;

  /// A simple vector class based on PETSc.
  ///
  /// It is a simple wrapper for a PETSc vector pointer (Vec)
  /// implementing the GenericTensor interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the PETSc Vec pointer using the function vec() and
  /// use the standard PETSc interface.

  class PETScVector : public PETScObject
  {
  public:

    /// Create empty vector on an MPI communicator
    explicit PETScVector(MPI_Comm comm);

    /// Copy constructor
    PETScVector(const PETScVector& x);

    /// Create vector wrapper of PETSc Vec pointer. The reference
    /// counter of the Vec will be increased, and decreased upon
    /// destruction of this object.
    explicit PETScVector(Vec x);

    /// Destructor
    virtual ~PETScVector();

    //--- Implementation of the GenericTensor interface ---

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply();

    /// Return MPI communicator
    virtual MPI_Comm mpi_comm() const;

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const;

    //--- Implementation of the PETScVector interface ---

    /// Return copy of vector
    virtual std::shared_ptr<PETScVector> copy() const;

    std::size_t size(std::size_t dim) const
    { return this->size(); }

     /// Return local ownership range
    std::pair<std::int64_t, std::int64_t> local_range(std::size_t dim) const
    { return this->local_range(); }

    /// Set block of values using global indices
    void set(const double* block, const dolfin::la_index_t* num_rows,
                     const dolfin::la_index_t * const * rows)
    { this->set(block, num_rows[0], rows[0]); }

    /// Set block of values using local indices
    void set_local(const double* block,
                           const dolfin::la_index_t* num_rows,
                           const dolfin::la_index_t * const * rows)
    { this->set_local(block, num_rows[0], rows[0]); }

    /// Add block of values using global indices
    virtual void add(const double* block, const dolfin::la_index_t* num_rows,
                     const dolfin::la_index_t * const * rows)
    { add(block, num_rows[0], rows[0]); }

    /// Add block of values using local indices
    virtual void add_local(const double* block,
                           const dolfin::la_index_t* num_rows,
                           const dolfin::la_index_t * const * rows)
    { add_local(block, num_rows[0], rows[0]); }

    /// Add block of values using global indices
    virtual void
      add(const double* block,
          const std::vector<ArrayView<const dolfin::la_index_t>>& rows)
    { add(block, rows[0].size(), rows[0].data()); }

    /// Add block of values using local indices
    virtual void
      add_local(const double* block,
                const std::vector<ArrayView<const dolfin::la_index_t>>& rows)
    { add_local(block, rows[0].size(), rows[0].data()); }

    std::size_t rank() const
    { return 1;}

    /// Initialize vector to global size N
    virtual void init(std::size_t N);

    /// Initialize vector with given ownership range
    virtual void init(std::array<std::int64_t, 2> range);

    /// Initialize vector with given ownership range and with ghost
    /// values
    virtual void init(std::array<std::int64_t, 2> range,
                      const std::vector<la_index_t>& local_to_global_map,
                      const std::vector<la_index_t>& ghost_indices,
                      int block_size);

    /// Return true if vector is empty
    virtual bool empty() const;

    /// Return size of vector
    virtual std::size_t size() const;

    /// Return local size of vector
    virtual std::size_t local_size() const;

    /// Return ownership range of a vector
    virtual std::pair<std::int64_t, std::int64_t> local_range() const;

    /// Determine whether global vector index is owned by this process
    virtual bool owns_index(std::size_t i) const;

    /// Get block of values using global indices (all values must be
    /// owned by local process, ghosts cannot be accessed)
    virtual void get(double* block, std::size_t m,
                     const dolfin::la_index_t* rows) const;

    /// Get block of values using local indices
    virtual void get_local(double* block, std::size_t m,
                           const dolfin::la_index_t* rows) const;

    /// Set block of values using global indices
    virtual void set(const double* block, std::size_t m,
                     const dolfin::la_index_t* rows);

    /// Set block of values using local indices
    virtual void set_local(const double* block, std::size_t m,
                           const dolfin::la_index_t* rows);

    /// Add block of values using global indices
    virtual void add(const double* block, std::size_t m,
                     const dolfin::la_index_t* rows);

    /// Add block of values using local indices
    virtual void add_local(const double* block, std::size_t m,
                           const dolfin::la_index_t* rows);

    /// Get all values on local process
    virtual void get_local(std::vector<double>& values) const;

    /// Set all values on local process
    virtual void set_local(const std::vector<double>& values);

    /// Add values to each entry on local process
    virtual void add_local(const std::vector<double>& values);

    /// Gather entries (given by global indices) into local
    /// (MPI_COMM_SELF) vector x. Provided x must be empty
    /// or of correct dimension (same as provided indices).
    /// This operation is collective
    virtual void gather(PETScVector& y,
                        const std::vector<dolfin::la_index_t>& indices) const;

    /// Gather entries (given by global indices) into x.
    /// This operation is collective
    virtual void gather(std::vector<double>& x,
                        const std::vector<dolfin::la_index_t>& indices) const;

    /// Gather all entries into x on process 0.
    /// This operation is collective
    virtual void gather_on_zero(std::vector<double>& x) const;

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const PETScVector& x);

    /// Replace all entries in the vector by their absolute values
    virtual void abs();

    /// Return dot product with given vector
    virtual double dot(const PETScVector& v) const;

    /// Return norm of vector
    virtual double norm(std::string norm_type) const;

    /// Return minimum value of vector
    virtual double min() const;

    /// Return maximum value of vector
    virtual double max() const;

    /// Return sum of values of vector
    virtual double sum() const;

    /// Multiply vector by given number
    virtual const PETScVector& operator*= (double a);

    /// Multiply vector by another vector pointwise
    virtual const PETScVector& operator*= (const PETScVector& x);

    /// Divide vector by given number
    virtual const PETScVector& operator/= (double a);

    /// Add given vector
    virtual const PETScVector& operator+= (const PETScVector& x);

    /// Add number to all components of a vector
    virtual const PETScVector& operator+= (double a);

    /// Subtract given vector
    virtual const PETScVector& operator-= (const PETScVector& x);

    /// Subtract number from all components of a vector
    virtual const PETScVector& operator-= (double a);

    /// Assignment operator
    virtual const PETScVector& operator= (const PETScVector& x);

    /// Assignment operator
    virtual const PETScVector& operator= (double a);

    /// Update values shared from remote processes
    virtual void update_ghost_values();

    //--- Special PETSc functions ---

    /// Sets the prefix used by PETSc when searching the options
    /// database
    void set_options_prefix(std::string options_prefix);

    /// Returns the prefix used by PETSc when searching the options
    /// database
    std::string get_options_prefix() const;

    /// Call PETSc function VecSetFromOptions on the underlying Vec
    /// object
    void set_from_options();

    /// Return pointer to PETSc Vec object
    Vec vec() const;

    /// Switch underlying PETSc object. Intended for internal library
    /// usage.
    void reset(Vec vec);

  private:

    // PETSc Vec pointer
    Vec _x;

  };

}

#endif

#endif
