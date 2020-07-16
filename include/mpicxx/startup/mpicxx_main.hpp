/**
 * @file 
 * @author Marcel Breyer
 * @date 2020-07-16
 * @copyright This file is distributed under the MIT License.
 *
 * @brief Implements a save way to setup and teardown the MPI environment, e.g. without the possibility to forget a call \n
 *        to *MPI_Init* or *MPI_Finalize*.
 */

#ifndef MPICXX_MPICXX_MAIN_HPP
#define MPICXX_MPICXX_MAIN_HPP

#include <mpicxx/detail/concepts.hpp>
#include <mpicxx/startup/finalize.hpp>
#include <mpicxx/startup/init.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>

namespace mpicxx {

    /// @name automatic initialization and finalization of the MPI environment
    ///@{
    /**
     * @brief Correctly setup and teardown the MPI environment while executing the code given by @p func.
     * @details This function performs the following tasks in the given order:
     *          -# call @ref mpicxx::init()
     *          -# invoke the function represented by @p func (forwarding all additional parameters)
     *          -# call @ref mpicxx::finalize()
     *
     *          Example:
     *          @snippet examples/startup/init_and_finalize.cpp normal version without args and thread support
     *          is the same as:
     *          @snippet examples/startup/mpicxx_main.cpp mpicxx_main version without args and thread support
     * @tparam FuncPtr must meet the @ref detail::is_main_pointer requirements
     * @tparam Args a parameter pack representing the additional (optional) parameters
     * @param[in] func a callable holding the main code of the application
     * @param[in] args the additional parameters forwarded to the user defined function @p func
     * @return the result of the invocation of @p func
     *
     * @calls{
     * int MPI_Init(int *argc, char ***argv);    // exactly once
     * int MPI_Finalize();                       // exactly once
     * }
     */
    template <typename FuncPtr, typename... Args>
    requires detail::is_main_pointer<FuncPtr, Args...>
    inline int main(FuncPtr func, Args&&... args) {
        init();

        int ret = std::invoke(func, std::forward<Args>(args)...);

        finalize();
        return ret;
    }
    /**
     * @brief Correctly setup and teardown the MPI environment while executing the code given by @p func.
     * @details This function performs the following tasks in the given order:
     *          -# call @ref mpicxx::init(int& argc, char** argv)
     *          -# invoke the function represented by @p func (forwarding all additional parameters)
     *          -# call @ref mpicxx::finalize()
     *
     *          Example:
     *          @snippet examples/startup/init_and_finalize.cpp normal version with args and without thread support
     *          is the same as:
     *          @snippet examples/startup/mpicxx_main.cpp mpicxx_main version with args and without thread support
     * @tparam FuncPtr must meet the @ref detail::is_main_args_pointer requirements
     * @tparam Args a parameter pack representing the additional (optional) parameters
     * @param[in] func a callable holding the main code of the application
     * @param[inout] argc the number of command line parameters
     * @param[inout] argv the command line parameters
     * @param[in] args the additional parameters forwarded to the user defined function @p func
     * @return the result of the invocation of @p func
     *
     * @calls{
     * int MPI_Init(int *argc, char ***argv);    // exactly once
     * int MPI_Finalize();                       // exactly once
     * }
     */
    template <typename FuncPtr, typename... Args>
    requires detail::is_main_args_pointer<FuncPtr, Args...>
    inline int main(FuncPtr func, int& argc, char** argv, Args&&... args) {
        init(argc, argv);

        int ret = std::invoke(func, argc, argv, std::forward<Args>(args)...);

        finalize();
        return ret;
    }

    /**
     * @brief Correctly setup and teardown the MPI environment with the required level of thread support while executing the code given by
     *        @p func. \n
     *        If the required level of thread support couldn't be satisfied, the function returns immediately with return code
     *        [`EXIT_FAILURE`](https://en.cppreference.com/w/cpp/utility/program/EXIT_status).
     * @details This function performs the following tasks in the given order:
     *          -# call @ref mpicxx::init(const thread_support)
     *          -# invoke the function represented by @p func (forwarding all additional parameters)
     *          -# call @ref mpicxx::finalize()
     *
     *          Example:
     *          @snippet examples/startup/init_and_finalize.cpp normal version without args and with thread support
     *          is nearly the same as (except for the return value):
     *          @snippet examples/startup/mpicxx_main.cpp mpicxx_main version without args and with thread support
     * @tparam FuncPtr must meet the @ref detail::is_main_pointer requirements
     * @tparam Args a parameter pack representing the additional (optional) parameters
     * @param[in] func a callable holding the main code of the application
     * @param[in] required the required level of thread support
     * @param[in] args the additional parameters forwarded to the user defined function @p func
     * @return the result of the invocation of @p func or [`EXIT_FAILURE`](https://en.cppreference.com/w/cpp/utility/program/EXIT_status)
     *         if the required level of thread support couldn't be satisfied
     *
     * @calls{
     * int MPI_Init_thread(int *argc, char ***argv, int required, int *provided);    // exactly once
     * int MPI_Finalize();                                                           // exactly once
     * }
     */
    template <typename FuncPtr, typename... Args>
    requires detail::is_main_pointer<FuncPtr, Args...>
    inline int main(FuncPtr func, const thread_support required, Args&&... args) {
        int ret = EXIT_FAILURE;
        try {
            init(required);
            ret = std::invoke(func, std::forward<Args>(args)...);
        } catch (const mpicxx::thread_support_not_satisfied& e) {
            std::cerr << e.what() << std::endl;
        }

        finalize();
        return ret;
    }
    /**
     * @brief Correctly setup and teardown the MPI environment with the required level of thread support while executing the code given by
     *        @p func. \n
     *        If the required level of thread support couldn't be satisfied, the function returns immediately with return code
     *        [`EXIT_FAILURE`](https://en.cppreference.com/w/cpp/utility/program/EXIT_status).
     * @details This function performs the following tasks in the given order:
     *          -# call @ref mpicxx::init(int& argc, char** argv, const thread_support)
     *          -# invoke the function represented by @p func (forwarding all additional parameters)
     *          -# call @ref mpicxx::finalize()
     *
     *          Example:
     *          @snippet examples/startup/init_and_finalize.cpp normal version with args and thread support
     *          is nearly the same as (except for the return value):
     *          @snippet examples/startup/mpicxx_main.cpp mpicxx_main version with args and thread support
     * @tparam FuncPtr must meet the @ref detail::is_main_pointer requirements
     * @tparam Args a parameter pack representing the additional (optional) parameters
     * @param[in] func a callable holding the main code of the application
     * @param[inout] argc the number of command line parameters
     * @param[inout] argv the command line parameters
     * @param[in] required the required level of thread support
     * @param[in] args the additional parameters forwarded to the user defined function @p func
     * @return the result of the invocation of @p func or [`EXIT_FAILURE`](https://en.cppreference.com/w/cpp/utility/program/EXIT_status)
     *         if the required level of thread support couldn't be satisfied
     *
     * @calls{
     * int MPI_Init_thread(int *argc, char ***argv, int required, int *provided);    // exactly once
     * int MPI_Finalize();                                                           // exactly once
     * }
     */
    template <typename FuncPtr, typename... Args>
    requires detail::is_main_args_pointer<FuncPtr, Args...>
    inline int main(FuncPtr func, int& argc, char** argv, const thread_support required, Args&&... args) {
        int ret = EXIT_FAILURE;
        try {
            init(argc, argv, required);
            ret = std::invoke(func, argc, argv, std::forward<Args>(args)...);
        } catch (const mpicxx::thread_support_not_satisfied& e) {
            std::cerr << e.what() << std::endl;
        }

        finalize();
        return ret;
    }
    ///@}

}

#endif // MPICXX_MPICXX_MAIN_HPP