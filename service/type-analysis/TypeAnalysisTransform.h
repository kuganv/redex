/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "GlobalTypeAnalyzer.h"
#include "IRCode.h"
#include "LocalTypeAnalyzer.h"
#include "WholeProgramState.h"

namespace type_analyzer {

/**
 * Optimize the given code by:
 *   - removing dead nonnull assertions generated by Kotlin
 * (checkParameterIsNotNull/checkExpressionValueIsNotNull)
 */
class Transform final {
 public:
  using NullAssertionSet = std::unordered_set<DexMethodRef*>;
  struct Config {
    size_t max_global_analysis_iteration{10};
    bool remove_dead_null_check_insn{true};
    Config() {}
  };

  struct Stats {
    size_t null_check_insn_removed{0};

    Stats& operator+=(const Stats& that) {
      null_check_insn_removed += that.null_check_insn_removed;
      return *this;
    }

    void report(PassManager& mgr) const {
      mgr.incr_metric("null_check_insn_removed", null_check_insn_removed);
      TRACE(TYPE_TRANSFORM, 2, "TypeAnalysisTransform Stats:");
      TRACE(TYPE_TRANSFORM,
            2,
            "TypeAnalysisTransform insns removed = %u",
            null_check_insn_removed);
    }
  };

  explicit Transform(Config config = Config()) : m_config(config) {}
  Stats apply(const type_analyzer::local::LocalTypeAnalyzer& lta,
              IRCode* code,
              const NullAssertionSet& null_assertion_set);
  static void setup(NullAssertionSet& null_assertion_set);

 private:
  void apply_changes(IRCode*);

  const Config m_config;
  std::vector<IRList::iterator> m_deletes;
};

} // namespace type_analyzer
