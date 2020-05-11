/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#pragma once

#include <ocs2_core/automatic_differentiation/CppAdInterface.h>
#include "ocs2_core/cost/CostFunctionBase.h"

namespace ocs2 {

/**
 * Cost Function Base with Algorithmic Differentiation (i.e. Auto Differentiation).
 */
class CostFunctionBaseAD : public CostFunctionBase {
 public:
  using ad_interface_t = CppAdInterface<scalar_t>;
  using ad_scalar_t = typename ad_interface_t::ad_scalar_t;
  using ad_dynamic_vector_t = typename ad_interface_t::ad_dynamic_vector_t;

  /**
   * Default constructor
   *
   */
  explicit CostFunctionBaseAD(size_t state_dim, size_t input_dim, size_t intermediate_cost_dim, size_t terminal_cost_dim);

  /**
   * Copy constructor
   */
  CostFunctionBaseAD(const CostFunctionBaseAD& rhs);

  /**
   * Default destructor
   */
  virtual ~CostFunctionBaseAD() = default;

  /**
   * Initializes model libraries
   *
   * @param modelName : name of the generate model library
   * @param modelFolder : folder to save the model library files to
   * @param recompileLibraries : If true, the model library will be newly compiled. If false, an existing library will be loaded if
   * available.
   * @param verbose : print information.
   */
  void initialize(const std::string& modelName, const std::string& modelFolder = "/tmp/ocs2", bool recompileLibraries = true,
                  bool verbose = true);

  void setCurrentStateAndControl(const scalar_t& t, const vector_t& x, const vector_t& u) final;

  void getIntermediateCost(scalar_t& L) final;

  void getIntermediateCostDerivativeTime(scalar_t& dLdt) final;

  void getIntermediateCostDerivativeState(vector_t& dLdx) final;

  void getIntermediateCostSecondDerivativeState(matrix_t& dLdxx) final;

  void getIntermediateCostDerivativeInput(vector_t& dLdu) final;

  void getIntermediateCostSecondDerivativeInput(matrix_t& dLduu) final;

  void getIntermediateCostDerivativeInputState(matrix_t& dLdux) final;

  void getTerminalCost(scalar_t& Phi) final;

  void getTerminalCostDerivativeTime(scalar_t& dPhidt) final;

  void getTerminalCostDerivativeState(vector_t& dPhidx) final;

  void getTerminalCostSecondDerivativeState(matrix_t& dPhidxx) final;

 protected:
  /**
   * Gets a user-defined cost parameters, applied to the intermediate costs
   *
   * @param [in] time: Current time.
   * @return The cost function parameters at a certain time
   */
  virtual vector_t getIntermediateParameters(scalar_t time) const;

  /**
   * Number of parameters for the intermediate cost function.
   * This number must be remain constant after the model libraries are created
   *
   * @return number of parameters
   */
  virtual size_t getNumIntermediateParameters() const;

  /**
   * Gets a user-defined cost parameters, applied to the terminal costs
   *
   * @param [in] time: Current time.
   * @return The cost function parameters at a certain time
   */
  virtual vector_t getTerminalParameters(scalar_t time) const;

  /**
   * Number of parameters for the terminal cost function.
   * This number must be remain constant after the model libraries are created
   *
   * @return number of parameters
   */
  virtual size_t getNumTerminalParameters() const;

  /**
   * Interface method to the intermediate cost function. This method must be implemented by the derived class.
   *
   * @tparam scalar type. All the floating point operations should be with this type.
   * @param [in] time: time.
   * @param [in] state: state vector.
   * @param [in] input: input vector.
   * @param [in] parameters: parameter vector.
   * @param [out] costValue: cost value.
   */
  virtual void intermediateCostFunction(ad_scalar_t time, const ad_dynamic_vector_t& state, const ad_dynamic_vector_t& input,
                                        const ad_dynamic_vector_t& parameters, ad_scalar_t& costValue) const = 0;

  /**
   * Interface method to the terminal cost function. This method can be implemented by the derived class.
   *
   * @tparam scalar type. All the floating point operations should be with this type.
   * @param [in] time: time.
   * @param [in] state: state vector.
   * @param [in] parameters: parameter vector.
   * @param [out] costValue: cost value.
   */
  virtual void terminalCostFunction(ad_scalar_t time, const ad_dynamic_vector_t& state, const ad_dynamic_vector_t& parameters,
                                    ad_scalar_t& costValue) const;

 private:
  /**
   * Sets all the required CppAdCodeGenInterfaces
   */
  void setADInterfaces(const std::string& modelName, const std::string& modelFolder);

  /**
   * Create the forward model and derivatives.
   *
   * @param [in] verbose: display information.
   */
  void createModels(bool verbose);

  /**
   * Loads the forward model and derivatives if available. Constructs them otherwise.
   *
   * @param [in] verbose: display information
   */
  void loadModelsIfAvailable(bool verbose);

  std::unique_ptr<ad_interface_t> terminalADInterfacePtr_;
  std::unique_ptr<ad_interface_t> intermediateADInterfacePtr_;

  size_t state_dim_;
  size_t input_dim_;
  size_t intermediate_cost_dim_;
  size_t terminal_cost_dim_;

  // Intermediate cost
  bool intermediateDerivativesComputed_;
  vector_t intermediateParameters_;
  vector_t tapedTimeStateInput_;
  row_vector_t intermediateJacobian_;
  matrix_t intermediateHessian_;

  // Final cost
  bool terminalDerivativesComputed_;
  vector_t terminalParameters_;
  vector_t tapedTimeState_;
  row_vector_t terminalJacobian_;
  matrix_t terminalHessian_;
};

}  // namespace ocs2
