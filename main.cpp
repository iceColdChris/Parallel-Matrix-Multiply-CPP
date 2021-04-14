#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct ThreadHelper {
    ThreadHelper() = default;

    vector<int> A;
    vector<int> B;
    vector<int> *C = nullptr;
    int StartA{};
    int EndA{};
    int s{};
    int t{};
};


void multiply(const shared_ptr<ThreadHelper> &helper) {
    auto result = helper->C;

    for (auto i = helper->StartA; i < helper->EndA; i++) {
        for (auto j = 0; j < helper->t; j++) {
            int sum = 0;
            for (auto k = 0; k < helper->s; k++) {
                sum = sum + helper->A[i * helper->s + k] * helper->B[k * helper->t + j];
            }
            (*result)[i * helper->t + j] = sum;
        }
    }
}

vector<int> create_random_matrix(int r, int s) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution dist(-5, 5);

    int size = r * s;
    vector<int> matrix;
    matrix.reserve(size);

    for (auto i = 0; i < size; i++)
        matrix.push_back(dist(mt));
    return matrix;
}

vector<int> threaded_matrix_product(const vector<int> &A, const vector<int> &B, int r, int s, int t) {

    vector<int> C(r * t, 0);
    /*
     * Creates a thread helper that multiplies
     * over the first fourth of the matrix A
     */
    auto Th1 = make_shared<ThreadHelper>();
    Th1->A = A;
    Th1->B = B;
    Th1->C = &C;
    Th1->StartA = 0;
    Th1->EndA = r / 4;
    Th1->s = s;
    Th1->t = t;

    /*
     * Creates a thread helper that multiplies
     * over the second fourth of the matrix A
     */
    auto Th2 = make_shared<ThreadHelper>();
    Th2->A = A;
    Th2->B = B;
    Th2->C = &C;
    Th2->StartA = r / 4;
    Th2->EndA = r / 2;
    Th2->s = s;
    Th2->t = t;

    /*
     * Creates a thread helper that multiplies
     * over the third fourth of the matrix A
     */
    auto Th3 = make_shared<ThreadHelper>();
    Th3->A = A;
    Th3->B = B;
    Th3->C = &C;
    Th3->StartA = r / 2;
    Th3->EndA = r - (r / 4);
    Th3->s = s;
    Th3->t = t;

    /*
     * Creates a thread helper that multiplies
     * over the remaining rows of the matrix A
     */
    auto Th4 = make_shared<ThreadHelper>();
    Th4->A = A;
    Th4->B = B;
    Th4->C = &C;
    Th4->StartA = r - (r / 4);
    Th4->EndA = r;
    Th4->s = s;
    Th4->t = t;

    shared_ptr<ThreadHelper> storage[] = {Th1, Th2, Th3, Th4};
    vector<jthread> threads;

    for (const auto &i : storage)
        threads.emplace_back([storage, &i]() {
            multiply(i);
        });

    for (auto &element : threads) {
        element.join();
    }

    return C;
}

/*
 * Test's the matrix multiplication algorithm
 * for correctness using smaller numbers.This
 * code was moved to account for performance
 * issues due to the conflict between this code and
 * the test speed function.
 *
 * Prints out
 * -----------
 * Matrix A
 * Matrix B
 * Matrix A*B
 */
void testMatrixMultiply() {
    const int r = 2;
    const int s = 2;
    const int t = 2;
    vector<int> A = create_random_matrix(r, s);
    vector<int> B = create_random_matrix(s, t);

    for (auto i: A)
        cout << i << ' ';
    cout << endl;

    for (auto i: B)
        cout << i << ' ';
    cout << endl;


    vector<int> C = threaded_matrix_product(A, B, r, s, t);

    for (auto i: C)
        cout << i << ' ';
    cout << endl;
}

/*
 * The default code from main moved into a
 * new method. This code was unedited just
 * moved to account for performance issues
 * due to the conflict between this code and
 * the test matrix multiply.
 */
void testSpeed() {
    const int r = 1000;
    const int s = 2000;
    const int t = 1000;

    vector<int> A = create_random_matrix(r, s);
    vector<int> B = create_random_matrix(s, t);

    auto start = high_resolution_clock::now();
    vector<int> C = threaded_matrix_product(A, B, r, s, t);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<seconds>(stop - start);

    cout << "Time taken to multiply: "
         << duration.count() << " seconds" << endl;

}

int main() {
    /* TODO Change These
     * Only have one of these running at a time
     * If you have both the test matrix multiply and test
     * speed it affects performance.
     */
    testMatrixMultiply();
//    testSpeed();
    return 0;
}