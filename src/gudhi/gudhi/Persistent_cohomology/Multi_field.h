/*    This file is part of the Gudhi Library. The Gudhi library
 *    (Geometric Understanding in Higher Dimensions) is a generic C++
 *    library for computational topology.
 *
 *    Author(s):       Clément Maria
 *
 *    Copyright (C) 2014  INRIA Sophia Antipolis-Méditerranée (France)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_PERSISTENT_COHOMOLOGY_INCLUDE_GUDHI_PERSISTENT_COHOMOLOGY_MULTI_FIELD_H_
#define SRC_PERSISTENT_COHOMOLOGY_INCLUDE_GUDHI_PERSISTENT_COHOMOLOGY_MULTI_FIELD_H_

#include <gmpxx.h>

#include <vector>
#include <utility>

namespace Gudhi {

namespace persistent_cohomology {

/** \brief Structure representing coefficients in a set of finite fields simultaneously
 * using the chinese remainder theorem.
 *
 * \implements CoefficientField
 * \ingroup persistent_cohomology

 * Details on the algorithms may be found in \cite boissonnat:hal-00922572
 */
class Multi_field {
 public:
  typedef mpz_class Element;

  Multi_field()
  : prod_characteristics_(0),
    mult_id_all(0),
    add_id_all(0) {
  }

  /* Initialize the multi-field. The generation of prime numbers might fail with
   * a very small probability.*/
  void init(int min_prime, int max_prime) {
    if (max_prime < 2) {
      std::cerr << "There is no prime less than " << max_prime << std::endl;
    }
    if (min_prime > max_prime) {
      std::cerr << "No prime in [" << min_prime << ":" << max_prime << "]"
          << std::endl;
    }
    // fill the list of prime numbers
    int curr_prime = min_prime;
    mpz_t tmp_prime;
    mpz_init_set_ui(tmp_prime, min_prime);
    // test if min_prime is prime
    int is_prime = mpz_probab_prime_p(tmp_prime, 25);  // probabilistic primality test

    if (is_prime == 0) {  // min_prime is composite
      mpz_nextprime(tmp_prime, tmp_prime);
      curr_prime = mpz_get_ui(tmp_prime);
    }

    while (curr_prime <= max_prime) {
      primes_.push_back(curr_prime);
      mpz_nextprime(tmp_prime, tmp_prime);
      curr_prime = mpz_get_ui(tmp_prime);
    }
    mpz_clear(tmp_prime);
    // set m to primorial(bound_prime)
    prod_characteristics_ = 1;
    for (auto p : primes_) {
      mpz_mul_ui(prod_characteristics_.get_mpz_t(),
                 prod_characteristics_.get_mpz_t(), p);
    }

    // Uvect_
    Element Ui;
    Element tmp_elem;
    for (auto p : primes_) {
      assert(p > 0);  // division by zero + non negative values
      tmp_elem = prod_characteristics_ / p;
      // Element tmp_elem_bis = 10;
      mpz_powm_ui(tmp_elem.get_mpz_t(), tmp_elem.get_mpz_t(), p - 1,
                  prod_characteristics_.get_mpz_t());
      Uvect_.push_back(tmp_elem);
    }
    mult_id_all = 0;
    for (auto uvect : Uvect_) {
      assert(prod_characteristics_ > 0);  // division by zero + non negative values
      mult_id_all = (mult_id_all + uvect) % prod_characteristics_;
    }
  }

  /** \brief Returns the additive idendity \f$0_{\Bbbk}\f$ of the field.*/
  const Element& additive_identity() const {
    return add_id_all;
  }
  /** \brief Returns the multiplicative identity \f$1_{\Bbbk}\f$ of the field.*/
  const Element& multiplicative_identity() const {
    return mult_id_all;
  }  // 1 everywhere

  Element multiplicative_identity(Element Q) {
    if (Q == prod_characteristics_) {
      return multiplicative_identity();
    }

    assert(prod_characteristics_ > 0);  // division by zero + non negative values
    Element mult_id = 0;
    for (unsigned int idx = 0; idx < primes_.size(); ++idx) {
      assert(primes_[idx] > 0);  // division by zero + non negative values
      if ((Q % primes_[idx]) == 0) {
        mult_id = (mult_id + Uvect_[idx]) % prod_characteristics_;
      }
    }
    return mult_id;
  }

  /** Returns y * w */
  Element times(const Element& y, const Element& w) {
    return plus_times_equal(0, y, w);
  }

  Element plus_equal(const Element& x, const Element& y) {
    return plus_times_equal(x, y, (Element)1);
  }

  /** \brief Returns the characteristic \f$p\f$ of the field.*/
  const Element& characteristic() const {
    return prod_characteristics_;
  }

  /** Returns the inverse in the field. Modifies P.*/
  std::pair<Element, Element> inverse(Element x, Element QS) {
    Element QR;
    mpz_gcd(QR.get_mpz_t(), x.get_mpz_t(), QS.get_mpz_t());  // QR <- gcd(x,QS)
    if (QR == QS)
      return std::pair<Element, Element>(additive_identity(), multiplicative_identity());  // partial inverse is 0
    Element QT = QS / QR;
    Element inv_qt;
    mpz_invert(inv_qt.get_mpz_t(), x.get_mpz_t(), QT.get_mpz_t());

    assert(prod_characteristics_ > 0);  // division by zero + non negative values
    return std::pair<Element, Element>(
        (inv_qt * multiplicative_identity(QT)) % prod_characteristics_, QT);
  }
  /** Returns -x * y.*/
  Element times_minus(const Element& x, const Element& y) {
    assert(prod_characteristics_ > 0);  // division by zero + non negative values
    return prod_characteristics_ - ((x * y) % prod_characteristics_);
  }

  /** Set x <- x + w * y*/
  Element plus_times_equal(const Element& x, const Element& y, const Element& w) {
    assert(prod_characteristics_ > 0);  // division by zero + non negative values
    Element result = (x + w * y) % prod_characteristics_;
    if (result < 0)
      result += prod_characteristics_;
    return result;
  }

  Element prod_characteristics_;  // product of characteristics of the fields
                                  // represented by the multi-field class
  std::vector<int> primes_;       // all the characteristics of the fields
  std::vector<Element> Uvect_;
  Element mult_id_all;
  const Element add_id_all;
};

}  // namespace persistent_cohomology

}  // namespace Gudhi

#endif  // SRC_PERSISTENT_COHOMOLOGY_INCLUDE_GUDHI_PERSISTENT_COHOMOLOGY_MULTI_FIELD_H_
