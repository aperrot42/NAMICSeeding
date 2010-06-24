#ifndef hungarian_algorithm_h_
#define hungarian_algorithm_h_

//:
// \file
// \author Amitha Perera
// \date   Sep 2004

#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>

//: Find the best column to row assignment given a cost matrix.
//
// This is an implementation of the Hungarian algorithm (also known
// as the Munkres algorithm). It finds the minimum cost assignment of
// the rows of the cost matrix \a cost (workers) to the columns
// (jobs).
//
// \param cost An N x M cost matrix. The costs cannot be -Infinity.
//
// \returns A vector v of size N such that v[i] = j means that row i
// should be assigned to column j. \code v[i] = -1u \endcode (= \code
// unsigned(-1) \endcode ) means that row i was not assigned to any
// column. If N \> M, then every column will be assigned to some
// row. If N \< M then every row will be assigned to some column.
//
//vcl_vector<unsigned> hungarian_algorithm( vnl_matrix<T> const& cost );

template <class T>
class hungarian_algorithm
{

  public:

    enum STEP_TYPE{
      STEP_0 = 0,
      STEP_1,
      STEP_2,
      STEP_3,
      STEP_4,
      STEP_5,
      STEP_6,
      STEP_done
    };

    enum STATE_TYPE{
      NORMAL=0,
      STAR,
      PRIME
    };

    typedef vnl_matrix< int >    AssignmentMatrixType;
    typedef vcl_vector<unsigned> AssignmentVectorType;

    hungarian_algorithm();
   ~hungarian_algorithm();

     /**
      *   \brief Starts the assignment
      */
     void SetCostMatrix( vnl_matrix< T > const& );


     /**
      *   \brief Starts the assignment
      */
    void StartAssignment();

    /**
     *   \brief Returns the total cost of the assignment
     */
    T                GetTotalCost();

    /**
     *   \brief Returns the assignment matrix
     */
    AssignmentMatrixType GetAssignmentMatrix();

    /**
     *   \brief Returns the assignment vector
     */
    AssignmentVectorType  GetAssignmentVector();


   private:

   protected:
    /**
     *   \brief Step 0 - Make sure there are at least as many rows as columns
     */
    void                  Step_0();

   /**
    *   \brief Step 1 - For each row of the matrix, find the smallest element
    *   and subtract it from every element in its row.  Go to Step 2.
    */
   void                  Step_1();

    /**
     *   \brief Step 2 - Find a zero (Z) in the resulting matrix.  If there is
     *   no starred zero in its row or column, star Z. Repeat for each element
     *   in the matrix. Go to Step 3.
     */
    void                  Step_2();

   /**
    *   \brief Step 3 - Cover each column containing a starred zero.  If K
    *    columns are covered, the starred zeros describe a complete set of
    *    unique assignments.  In this case, Go to DONE, otherwise, Go to
    *    Step 4.
    */
    STEP_TYPE          Step_3();

    /**
     *   \brief Step 4: Find a noncovered zero and prime it.  If there is no
     *   starred zero in the row containing this primed zero, Go to Step 5.
     *   Otherwise, cover this row and uncover the column containing the starred
     *   zero. Continue in this manner until there are no uncovered zeros left.
     *   Save the smallest uncovered value and Go to Step 6.
     */
    STEP_TYPE          Step_4();

   /**
    *   \brief Step 5 - Construct a series of alternating primed and starred
    *   zeros as follows.  Let Z0 represent the uncovered primed zero found in
    *   Step 4.  Let Z1 denote the starred zero in the column of Z0 (if any).
    *   Let Z2 denote the primed zero in the row of Z1 (there will always be
    *   one).  Continue until the series terminates at a primed zero that has
    *   no starred zero in its column.  Unstar each starred zero of the series,
    *   star each primed zero of the series, erase all primes and uncover every
    *   line in the matrix. Return to Step 3.
    */
    STEP_TYPE          Step_5();

   /**
    *   \brief Step 6 - Add the value found in Step 4 to every element of each
    *   covered row, and subtract it from every element of each uncovered
    *   column.  Return to Step 4 without altering any stars, primes, or
    *   covered lines.
    */
    STEP_TYPE          Step_6();

   /**
    *   \brief Step done - Returns a vector containing the result of the
    *   assignment.
    */
    void  Step_done();

   /**
    *   \brief Sets all the values in the input bool vector to false.
    */
    void clear_vector( vcl_vector<bool>& );

    vnl_matrix<T> m_Cost;
    vnl_matrix<T> m_Cost_in;

   /**
    *   \brief Size of the input matrix
    */
    unsigned m_N;

   /**
    *   \brief Row and col of the primed zero in step four to pass to step five.
    */
    unsigned m_Z0_r, m_Z0_c;

   /**
    *   \brief m_M(i,j) = 1   =>  m_Cost(i,j) is starred
    *          m_M(i,j) = 2   =>  m_Cost(i,j) is primed
    */
    AssignmentMatrixType m_M;

   /**
    *   \brief m_R_cov[i] = true  => row i is covered
    */
    vcl_vector<bool> m_R_cov;

    /**
     *   \brief m_C_cov[j] = true  => column j is covered
     */
    vcl_vector<bool> m_C_cov;

   /**
    *   \brief Total cost of the assignment
    */
    T m_TotalCost;

   /**
    *   \brief m_C_cov[j] = true  => column j is covered
    */
    AssignmentVectorType m_AssignmentVector;
};

#include "hungarian_algorithm.txx"

#endif // vnl_hungarian_algorithm_h_
