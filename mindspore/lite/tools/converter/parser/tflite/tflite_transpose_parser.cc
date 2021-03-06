/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tools/converter/parser/tflite/tflite_transpose_parser.h"
#include <vector>
#include <memory>

namespace mindspore {
namespace lite {
PrimitiveC *TfliteTransposeParser::ParseLitePrimitive(const std::unique_ptr<tflite::OperatorT> &tflite_op,
                                                      const std::unique_ptr<tflite::ModelT> &tflite_model) {
  auto &tflite_subgraph = tflite_model->subgraphs.front();
  auto primitive = std::make_unique<schema::PrimitiveT>();
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is null";
    return nullptr;
  }

  std::unique_ptr<schema::TransposeT> attr = std::make_unique<schema::TransposeT>();
  if (attr == nullptr) {
    MS_LOG(ERROR) << "new op failed";
    return nullptr;
  }

  if (GetTfliteData(tflite_op->inputs[1], tflite_subgraph->tensors, tflite_model->buffers, attr->perm)) {
    MS_LOG(ERROR) << "get transpose -> perm failed";
    return nullptr;
  }

  attr->conjugate = false;
  primitive->value.type = schema::PrimitiveType_Transpose;
  primitive->value.value = attr.release();
  return PrimitiveC::Create(primitive.release());
}

TfliteNodeRegister g_tfliteTransposeParser(tflite::BuiltinOperator_TRANSPOSE, new TfliteTransposeParser());
}  // namespace lite
}  // namespace mindspore
