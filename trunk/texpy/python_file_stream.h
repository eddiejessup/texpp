/*  *** License agreement ***
    
    cctbx Copyright (c) 2006, The Regents of the University of
    California, through Lawrence Berkeley National Laboratory (subject to
    receipt of any required approvals from the U.S. Dept. of Energy).  All
    rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
    (1) Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    
    (2) Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    
    (3) Neither the name of the University of California, Lawrence Berkeley
    National Laboratory, U.S. Dept. of Energy nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
    You are under no obligation whatsoever to provide any bug fixes,
    patches, or upgrades to the features, functionality or performance of
    the source code ("Enhancements") to anyone; however, if you choose to
    make your Enhancements available either publicly, or directly to
    Lawrence Berkeley National Laboratory, without imposing a separate
    written license agreement for such Enhancements, then you hereby grant
    the following license: a  non-exclusive, royalty-free perpetual license
    to install, use, modify, prepare derivative works, incorporate into
    other computer software, distribute, and sublicense such enhancements or
    derivative works thereof, in binary and source code form.
 */


#ifndef BOOST_ADAPTBX_PYTHON_FILE_STREAM_H
#define BOOST_ADAPTBX_PYTHON_FILE_STREAM_H

#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>

#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

#include <streambuf>
#include <iostream>

namespace boost_adaptbx { namespace file_conversion {

namespace python = boost::python;


/// A stream buffer getting data from and putting data into a Python file object
/** The aims are as follow:

    - Given a C++ function acting on a standard stream, e.g.

      \code
      void mundanely_read(std::istream &input) {
        ...
        input >> something >> something_else;
      }
      \endcode

      and given a piece of Python code which creates a file-object, to be
      able to pass this file object to that C++ function, e.g.

      \code
      import gzip
      gzip_file = gzip.GzipFile(...)
      mundanely_read(gzip_file)
      \endcode

      and have the standard stream pull data from and put data into the Python
      file object.

    - When Python \c mundanely_read returns, the Python object is able to
      continue reading or writing where the C++ code left off.

    - Operations in C++ on mere files should be competitively fast compared
      to the direct use of \c std::fstream.


    \b Motivation

      - the standard Python library offer of file-like objects (files,
        compressed files and archives, network, ...) is far superior to the
        offer of streams in the C++ standard library and Boost C++ libraries.

      - i/o code involves a fair amount of text processing which is more
        efficiently prototyped in Python but then one may need
        to rewrite a time-critical part in C++, in as seamless a manner
        as possible.

    \b Design

    This is 2-step.

      - a Boost.Python conversion from any object featuring enough file
        operations (read, write, seek) to this class, which can then be used
        as a stream buffer for the stream classes \c istream, \c ostream and
        \c iostream defined in namespace \c boost_adaptbx::file_conversion
        Written once, useable everywhere the Boost.Python extension module
        "python_file_ext" is loaded.

      - a trivial bridge function

        \code
        namespace boost_adaptbx { namespace file_conversion {

          void bridge(python_file_buffer const &input) {
            istream is(&input);
            mundanely_read(const_cast<istream &>(input));
            is.sync(); // synchs the python file object with 'is'
          }

        }}
        \endcode

        which has to be written every time one wants a Python binding for
        such a C++ function. Note that is.sync() shall be replaced by
        os.flush() for an output stream.

    Here are the rationales for this design:

      - it seems impossible to convert a file-object directly to std::istream,
        std::ostream or std::iostream because we can't control the lifetime
        of the stream buffer. We would create the latter in the conversion code
        but what code would destroy it? The std:*stream classes do indeed
        not manage the buffer lifetime.

      - the conversion system would not be triggered if the bridge function
        took an argument \code python_file_buffer &input \endcode. We need
        the const reference because Boost.Python does not have custom converters
        with write-back: if the result of the conversion was to be altered,
        then those modifications should be written back to the converted Python
        object so as not to create any surprise. Since that is not supported,
        const-references are enforced to discourage modifying the converted
        object. However in our case, we know what we are doing! That is,
        any operation on the instance of \c python_file_buffer
        is delegated to the associated Python file object:
        this is all sound and safe.

      - but then stream classes need a non-const pointer to a file buffer to be
        passed to their constructor (since they will modify that buffer).
        Hence the need for a const_cast to work around the mismatch between
        the handrail put by Boost.Python, which is over-conservative here,
        and the C++ colloquialisms. That const-cast is the only reason for the
        stream classes in boost_adaptbx::file_conversion to exist.

  Note: references are to the C++ standard (the numbers between parentheses
  at the end of references are margin markers).
*/
class python_file_buffer : public std::basic_streambuf<char>
{
  private:
    typedef std::basic_streambuf<char> base_t;

  public:
    /* The syntax
        using base_t::char_type;
       would be nicer but Visual Studio C++ 8 chokes on it
    */
    typedef base_t::char_type   char_type;
    typedef base_t::int_type    int_type;
    typedef base_t::pos_type    pos_type;
    typedef base_t::off_type    off_type;
    typedef base_t::traits_type traits_type;

    // work around Visual C++ 7.1 problem
    inline
    static
    int
    traits_type_eof() { return traits_type::eof(); }

    /// The size of the read and write buffer.
    /** They are respectively used to buffer data read from and data written to
        the Python file object. It can be modified from Python.
    */
    static std::size_t buffer_size;

    /// Construct from a Python file object
    python_file_buffer(python::object& python_file)
      : py_read (getattr(python_file, "read",  python::object())),
        py_write(getattr(python_file, "write", python::object())),
        py_seek (getattr(python_file, "seek",  python::object())),
        py_tell (getattr(python_file, "tell",  python::object())),
        write_buffer(0),
        farthest_pptr(0),
        pos_of_read_buffer_end_in_py_file(0),
        pos_of_write_buffer_end_in_py_file(buffer_size)
    {
      if (py_write != python::object()) {
        // C-like string to make debugging easier
        write_buffer = new char[buffer_size + 1];
        write_buffer[buffer_size] = '\0';
        setp(write_buffer, write_buffer + buffer_size);  // 27.5.2.4.5 (5)
        farthest_pptr = pptr();
      }
      else {
        // The first attempt at output will result in a call to overflow
        setp(0, 0);
      }
      if (py_tell != python::object()) {
        off_type py_pos = python::extract<off_type>(py_tell());
        pos_of_read_buffer_end_in_py_file = py_pos;
        pos_of_write_buffer_end_in_py_file = py_pos;
      }
    }


    /// Mundane destructor freeing the allocated resources
    virtual ~python_file_buffer() {
      this->sync();
      if (write_buffer) delete[] write_buffer;
    }

    /// C.f. C++ standard section 27.5.2.4.3
    virtual int_type underflow() {
      int_type const failure = traits_type::eof();
      if (py_read == python::object()) {
        PyErr_SetString(PyExc_AttributeError,
                        "That Python file object has no 'read' attribute");
        python::throw_error_already_set();
      }
      read_buffer = py_read(buffer_size);
      char *read_buffer_data;
      python::ssize_t py_n_read;
      if (PyString_AsStringAndSize(read_buffer.ptr(),
                                   &read_buffer_data, &py_n_read) == -1) {
        setg(0, 0, 0);
        PyErr_SetString(PyErr_Occurred(),
                        "The method 'read' of the Python file object "
                        "did not return a string.");
        python::throw_error_already_set();
      }
      off_type n_read = (off_type)py_n_read;
      pos_of_read_buffer_end_in_py_file += n_read;
      setg(read_buffer_data, read_buffer_data, read_buffer_data + n_read);
      // ^^^27.5.2.3.1 (4)
      if (n_read == 0) return failure;
      return traits_type::to_int_type(read_buffer_data[0]);
    }

    /// C.f. C++ standard section 27.5.2.4.5
    virtual int_type overflow(int_type c=traits_type_eof()) {
      if (py_write == python::object()) {
        PyErr_SetString(PyExc_AttributeError,
                        "That Python file object has no 'write' attribute");
        python::throw_error_already_set();
      }
      farthest_pptr = std::max(farthest_pptr, pptr());
      off_type n_written = (off_type)(farthest_pptr - pbase());
      python::str chunk(pbase(), farthest_pptr);
      py_write(chunk);
      if (!traits_type::eq_int_type(c, traits_type::eof())) {
        py_write(traits_type::to_char_type(c));
        n_written++;
      }
      if (n_written) {
        pos_of_write_buffer_end_in_py_file += n_written;
        setp(pbase(), epptr());
        // ^^^ 27.5.2.4.5 (5)
        farthest_pptr = pptr();
      }
      return traits_type::eq_int_type(
        c, traits_type::eof()) ? traits_type::not_eof(c) : c;
    }

    /// Update the python file to reflect the state of this stream buffer
    /** Empty the write buffer into the Python file object and set the seek
        position of the latter accordingly (C++ standard section 27.5.2.4.2).
        If there is no write buffer or it is empty, but there is a non-empty
        read buffer, set the Python file object seek position to the
        seek position in that read buffer.
    */
    virtual int sync() {
      int result = 0;
      farthest_pptr = std::max(farthest_pptr, pptr());
      if (farthest_pptr && farthest_pptr > pbase()) {
        off_type delta = pptr() - farthest_pptr;
        int_type status = overflow();
        if (traits_type::eq_int_type(status, traits_type::eof())) result = -1;
        if (py_seek != python::object()) py_seek(delta, 1);
      }
      else if (gptr() && gptr() < egptr()) {
        if (py_seek != python::object()) py_seek(gptr() - egptr(), 1);
      }
      return result;
    }

    /// C.f. C++ standard section 27.5.2.4.2
    /** This implementation is optimised to look whether the position is within
        the buffers, so as to avoid calling Python seek or tell. It is
        important for many applications that the overhead of calling into Python
        is avoided as much as possible (e.g. parsers which may do a lot of
        backtracking)
    */
    virtual
    pos_type seekoff(off_type off, std::ios_base::seekdir way,
                     std::ios_base::openmode which=  std::ios_base::in
                                                   | std::ios_base::out)
    {
      /* In practice, "which" is either std::ios_base::in or out
         since we end up here because either seekp or seekg was called
         on the stream using this buffer. That simplifies the code
         in a few places.
      */
      int const failure = off_type(-1);

      if (py_seek == python::object()) {
        PyErr_SetString(PyExc_AttributeError,
                        "That Python file object has no 'seek' attribute");
        python::throw_error_already_set();
      }

      // we need the read buffer to contain something!
      if (which == std::ios_base::in && !gptr()) {
        if (traits_type::eq_int_type(underflow(), traits_type::eof())) {
          return failure;
        }
      }

      // compute the whence parameter for Python seek
      int whence;
      switch (way) {
        case std::ios_base::beg:
          whence = 0;
          break;
        case std::ios_base::cur:
          whence = 1;
          break;
        case std::ios_base::end:
          whence = 2;
          break;
        default:
          return failure;
      }

      // Let's have a go
      boost::optional<off_type> result = seekoff_without_calling_python(
        off, way, which);
      if (!result) {
        // we need to call Python
        if (which == std::ios_base::out) overflow();
        if (way == std::ios_base::cur) {
          if      (which == std::ios_base::in)  off -= egptr() - gptr();
          else if (which == std::ios_base::out) off += pptr() - pbase();
        }
        py_seek(off, whence);
        result = off_type(python::extract<off_type>(py_tell()));
        if (which == std::ios_base::in) underflow();
      }
      return *result;
    }

    /// C.f. C++ standard section 27.5.2.4.2
    virtual
    pos_type seekpos(pos_type sp,
                     std::ios_base::openmode which=  std::ios_base::in
                                                   | std::ios_base::out)
    {
      return python_file_buffer::seekoff(sp, std::ios_base::beg, which);
    }

  private:
    python::object py_read, py_write, py_seek, py_tell;

    /* This is actually a Python string and the actual read buffer is
       its internal data, i.e. an array of characters. We use a Boost.Python
       object so as to hold on it: as a result, the actual buffer can't
       go away.
    */
    python::object read_buffer;

    /* A mere array of char's allocated on the heap at construction time and
       de-allocated only at destruction time.
    */
    char *write_buffer;
    char *farthest_pptr; // the farthest place the buffer has been written into

    off_type pos_of_read_buffer_end_in_py_file,
             pos_of_write_buffer_end_in_py_file;


    boost::optional<off_type> seekoff_without_calling_python(
      off_type off,
      std::ios_base::seekdir way,
      std::ios_base::openmode which)
    {
      boost::optional<off_type> const failure;

      // Buffer range and current position
      off_type buf_begin, buf_end, buf_cur, upper_bound;
      off_type pos_of_buffer_end_in_py_file;
      if (which == std::ios_base::in) {
        pos_of_buffer_end_in_py_file = pos_of_read_buffer_end_in_py_file;
        buf_begin = reinterpret_cast<std::streamsize>(eback());
        buf_cur = reinterpret_cast<std::streamsize>(gptr());
        buf_end = reinterpret_cast<std::streamsize>(egptr());
        upper_bound = buf_end;
      }
      else if (which == std::ios_base::out) {
        pos_of_buffer_end_in_py_file = pos_of_write_buffer_end_in_py_file;
        buf_begin = reinterpret_cast<std::streamsize>(pbase());
        buf_cur = reinterpret_cast<std::streamsize>(pptr());
        buf_end = reinterpret_cast<std::streamsize>(epptr());
        farthest_pptr = std::max(farthest_pptr, pptr());
        upper_bound = reinterpret_cast<std::streamsize>(farthest_pptr) + 1;
      }

      // Sought position in "buffer coordinate"
      off_type buf_sought;
      if (way == std::ios_base::cur) {
        buf_sought = buf_cur + off;
      }
      else if (way == std::ios_base::beg) {
        buf_sought = buf_end + (off - pos_of_buffer_end_in_py_file);
      }

      // if the sought position is not in the buffer, give up
      if (buf_sought < buf_begin || buf_sought >= upper_bound) return failure;

      // we are in wonderland
      if      (which == std::ios_base::in)  gbump(buf_sought - buf_cur);
      else if (which == std::ios_base::out) pbump(buf_sought - buf_cur);
      return pos_of_buffer_end_in_py_file + (buf_sought - buf_end);
    }
};


namespace details {
  template <class BaseType>
  class gen_stream : public BaseType
  {
    public:
      gen_stream(std::auto_ptr<python_file_buffer> sb)
        : BaseType(sb.get())
      {
        m_sb = sb;
        this->exceptions(std::ios_base::badbit);
      }
      ~gen_stream()
      {
      }
     protected:
        std::auto_ptr<python_file_buffer> m_sb;
    };
}


typedef details::gen_stream<std::istream>  istream;
typedef details::gen_stream<std::ostream>  ostream;
typedef details::gen_stream<std::iostream> iostream;

}} // boost_adaptbx::file

#endif // GUARD

