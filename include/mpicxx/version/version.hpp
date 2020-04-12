/**
 * @file include/mpicxx/version/version.hpp
 * @author Marcel Breyer
 * @date 2020-03-17
 *
 * @brief Implements functions to query the current mpicxx and MPI version.
 */

#ifndef MPICXX_VERSION_HPP
#define MPICXX_VERSION_HPP

#include <string_view>

#include <mpi.h>
#include <fmt/format.h>


namespace mpicxx::version {

    /// @name version details specific to the mpicxx library
    ///@{
    /**
     * @brief The name of the mpicxx library.
     * @details The value gets automatically configured with the library name specified in the main cmake file.
     */
    constexpr std::string_view name = "mpicxx";
    /**
     * @brief The current version of the mpicxx library.
     * @details The value gets automatically configured with the library version specified in the main cmake file. \n
     * It has the form: "version_major.version_minor.version_patch".
     */
    constexpr std::string_view version = "0.3.1";
    /**
     * @brief The current major version of the mpicxx library.
     * @details The value gets automatically configured with the library major version specified in the main cmake file.
     */
    constexpr int version_major = 0;
    /**
     * @brief The current minor version of the mpicxx library.
     * @details The value gets automatically configured with the library minor version specified in the main cmake file.
     */
    constexpr int version_minor = 3;
    /**
     * @brief The current patch version of the mpicxx library.
     * @details The value gets automatically configured with the library patch version specified in the main cmake file.
     */
    constexpr int version_patch = 1;
    ///@}

    /// @name version details specific to the used MPI standard
    ///@{
    namespace detail {
        /*
         * @brief The current version of the used MPI standard.
         * @details The only reason this function exists is that all version constants and function can return a std::string_view.
         * @return a pair containing the major and minor MPI standard version
         *
         * @calls{
         * int MPI_Get_version(int *version, int *subversion);      // exactly once
         * }
         */
        std::pair<int, int> get_mpi_version() {
            int version, subversion;
            MPI_Get_version(&version, &subversion);
            return std::make_pair(version, subversion);
        }
    }
    /**
     * @brief The current version of the used MPI standard in the form "mpi_version_major.mpi_version_minor" (e.g. `"3.1"`).
     * @details This function can be called before @ref mpicxx::init() and after @ref mpicxx::finalize() and is thread safe as required by
     * the [MPI standard 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).
     * @return the MPI standard version
     *
     * @calls{
     * int MPI_Get_version(int *version, int *subversion);      // exactly twice (through all function calls)
     * }
     */
     std::string_view mpi_version() {
         // TODO 2020-03-15 15:37 marcel: change from fmt::format to std::format
         static const std::string version = fmt::format("{}.{}", detail::get_mpi_version().first, detail::get_mpi_version().second);
         return std::string_view(version);
     }
    /**
     * @brief The current major version of the used MPI standard.
     * @details This function can be called before @ref mpicxx::init() and after @ref mpicxx::finalize() and is thread safe as required by
     * the [MPI standard 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).
     * @return the MPI standard version
     *
     * @calls{
     * int MPI_Get_version(int *version, int *subversion);      // exactly once
     * }
     */
     int mpi_version_major() {
         return detail::get_mpi_version().first;
     }
    /**
     * @brief The current minor version (subversion) of the used MPI standard.
     * @details This function can be called before @ref mpicxx::init() and after @ref mpicxx::finalize() and is thread safe as required by
     * the [MPI standard 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).
     * @return the MPI standard subversion
     *
     * @calls{
     * int MPI_Get_version(int *version, int *subversion);      // exactly once
     * }
     */
     int mpi_version_minor() {
         return detail::get_mpi_version().second;
     }
    ///@}

    /// @name version details specific to the used MPI library
    ///@{
    namespace detail {
        /*
         * @brief The current version of the used MPI library.
         * @details The only reason this function exists is that all version constants and function can return a std::string_view.
         * @return a library specific version string
         *
         * @calls{
         * int MPI_Get_library_version(char *version, int *resultlen);      // exactly once
         * }
         */
        std::string get_mpi_library_version() {
            char library_version[MPI_MAX_LIBRARY_VERSION_STRING];
            int length;
            MPI_Get_library_version(library_version, &length);
            return std::string(library_version, length);
        }
        /*
         * @brief The name of the used MPI library.
         * @details
         * @return the name of the used MPI library or `"unknown"`
         *
         * @calls{
         * int MPI_Get_library_version(char *version, int *resultlen);      // exactly once
         * }
         */
        std::string get_mpi_library_name() {
            std::string library_version = detail::get_mpi_library_version();
            if (library_version.find("Open MPI") != std::string::npos) {
                return std::string("Open MPI");
            } else if (library_version.find("MPICH") != std::string::npos) {
                return std::string("MPICH");
            } else {
                return std::string("unknown");
            }
        }
    }
    /**
     * @brief The name of the used MPI library.
     * @details The value is one of: `"Open MPI"`, `"MPICH"`, or `"unknown"`.
     *
     * This function can be called before @ref mpicxx::init() and after @ref mpicxx::finalize() and is thread safe as required by
     * the [MPI standard 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).
     * @return the name of the used MPI library
     *
     * @calls{
     * int MPI_Get_library_version(char *version, int *resultlen);      // exactly once (through all function calls)
     * }
     */
    std::string_view mpi_library_name() {
        static const std::string library_name = detail::get_mpi_library_name();
        return std::string_view(library_name);
    }
    /**
     * @brief The current version of the used MPI library (library specific implementation defined).
     * @details This function can be called before @ref mpicxx::init() and after @ref mpicxx::finalize() and is thread safe as required by
     * the [MPI standard 3.1](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report.pdf).
     * @return a library specific version string
     *
     * @calls{
     * int MPI_Get_library_version(char *version, int *resultlen);      // exactly once (through all function calls)
     * }
     */
    std::string_view mpi_library_version() {
        static const std::string library_version = detail::get_mpi_library_version();
        return std::string_view(library_version);
    }
    ///@}

}


#endif // MPICXX_VERSION_HPP