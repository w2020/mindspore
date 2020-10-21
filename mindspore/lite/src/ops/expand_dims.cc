/**
 * Copyright 2019-2020 Huawei Technologies Co., Ltd
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

#include "src/ops/expand_dims.h"

#include "src/ops/ops_register.h"
#include "nnacl/fp32/expandDims.h"

namespace mindspore {
namespace lite {
#ifdef PRIMITIVE_WRITEABLE
int ExpandDims::GetDim() const { return this->primitive_->value.AsExpandDims()->dim; }

void ExpandDims::SetDim(int dim) { this->primitive_->value.AsExpandDims()->dim = dim; }

#else
int ExpandDims::UnPackToFlatBuilder(const schema::Primitive *primitive, flatbuffers::FlatBufferBuilder *fbb) {
  MS_ASSERT(nullptr != primitive);
  MS_ASSERT(nullptr != fbb);
  auto attr = primitive->value_as_ExpandDims();
  if (attr == nullptr) {
    MS_LOG(ERROR) << "value_as_ExpandDims return nullptr";
    return RET_ERROR;
  }

  auto val_offset = schema::CreateExpandDims(*fbb, attr->dim());
  auto prim_offset = schema::CreatePrimitive(*fbb, schema::PrimitiveType_ExpandDims, val_offset.o);
  fbb->Finish(prim_offset);
  return RET_OK;
}
int ExpandDims::GetDim() const { return this->primitive_->value_as_ExpandDims()->dim(); }

PrimitiveC *ExpandDimsCreator(const schema::Primitive *primitive) {
  return PrimitiveC::NewPrimitiveC<ExpandDims>(primitive);
}
Registry ExpandDimsRegistry(schema::PrimitiveType_ExpandDims, ExpandDimsCreator);
#endif

OpParameter *PopulateExpandDimsParameter(const mindspore::lite::PrimitiveC *primitive) {
  auto param = reinterpret_cast<mindspore::lite::ExpandDims *>(const_cast<mindspore::lite::PrimitiveC *>(primitive));
  ExpandDimsParameter *expand_dims_param = reinterpret_cast<ExpandDimsParameter *>(malloc(sizeof(ExpandDimsParameter)));
  if (expand_dims_param == nullptr) {
    MS_LOG(ERROR) << "malloc ExpandDimsParameter failed.";
    return nullptr;
  }
  memset(expand_dims_param, 0, sizeof(ExpandDimsParameter));
  expand_dims_param->op_parameter_.type_ = primitive->Type();
  expand_dims_param->dim_ = param->GetDim();
  return reinterpret_cast<OpParameter *>(expand_dims_param);
}

Registry ExpandDimsParameterRegistry(schema::PrimitiveType_ExpandDims, PopulateExpandDimsParameter);

int ExpandDims::InferShape(std::vector<Tensor *> inputs_, std::vector<Tensor *> outputs_) {
  MS_ASSERT(this->primitive_ != nullptr);
  auto input = inputs_.front();
  MS_ASSERT(input != nullptr);
  auto output = outputs_.front();
  MS_ASSERT(output != nullptr);
  if (inputs_.size() != kSingleNum) {
    MS_LOG(ERROR) << "input size is invalid";
  }
  if (outputs_.size() != kSingleNum) {
    MS_LOG(ERROR) << "output size is invalid";
  }
  output->set_data_type(input->data_type());
  output->SetFormat(input->GetFormat());
  if (!GetInferFlag()) {
    return RET_OK;
  }
  int dim = GetDim();
  if (dim < 0) {
    dim += input->shape().size() + 1;
  }
  if (dim > static_cast<int>(input->shape().size())) {
    MS_LOG(ERROR) << "attribute dim out of range";
    return RET_INPUT_TENSOR_ERROR;
  }
  auto out_shape = input->shape();
  out_shape.insert(out_shape.begin() + dim, 1, 1);
  output->set_shape(out_shape);
  return RET_OK;
}
}  // namespace lite
}  // namespace mindspore
