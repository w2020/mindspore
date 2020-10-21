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

#include "src/ops/equal.h"

#include "src/ops/ops_register.h"

namespace mindspore {
namespace lite {
#ifndef PRIMITIVE_WRITEABLE
int Equal::UnPackToFlatBuilder(const schema::Primitive *primitive, flatbuffers::FlatBufferBuilder *fbb) {
  MS_ASSERT(nullptr != primitive);
  MS_ASSERT(nullptr != fbb);
  auto val_offset = schema::CreateEqual(*fbb);
  auto prim_offset = schema::CreatePrimitive(*fbb, schema::PrimitiveType_Equal, val_offset.o);
  fbb->Finish(prim_offset);
  return RET_OK;
}

PrimitiveC *EqualCreator(const schema::Primitive *primitive) { return PrimitiveC::NewPrimitiveC<Equal>(primitive); }
Registry EqualRegistry(schema::PrimitiveType_Equal, EqualCreator);
#endif
int Equal::InferShape(std::vector<Tensor *> inputs_, std::vector<Tensor *> outputs_) {
  auto input = inputs_.front();
  MS_ASSERT(input != nullptr);
  auto output = outputs_.front();
  MS_ASSERT(output != nullptr);
  output->set_shape(input->shape());
  output->set_data_type(TypeId::kNumberTypeBool);
  output->SetFormat(input->GetFormat());
  return RET_OK;
}
Registry EqualParameterRegistry(schema::PrimitiveType_Equal, PopulateArithmetic);
}  // namespace lite
}  // namespace mindspore
