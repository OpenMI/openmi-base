/*!
 *  Copyright (c) 2016 by Contributors
 *  \file protobuf_op.h
 *  \brief protobuf op. io/convert/...
 */
#ifndef OPENMI_BASE_PROTOBUF_OP_H_
#define OPENMI_BASE_PROTOBUF_OP_H_ 

#include <fcntl.h>   // open
#include <memory>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "base/logging.h"

using namespace openmi;

namespace openmi {

/*!
 * code:
  std::shared_ptr<ExtractorConf> conf_ = std::make_shared<ExtractorConf>();
  ProtobufOp::LoadObjectFromPbFile<ExtractorConf>("./conf/extractor.conf", conf_.get());
  LOG(INFO) << "feature_list: " << conf_->feature_list_file();
*/
class ProtobufOp {
public:
  template<typename PbType>
  static int LoadObjectFromPbFile(const char* file, PbType* obj) {
    int file_fd = open(file, O_RDONLY);
    if (file_fd < 0) {
      LOG(ERROR) << "open pb file failed. file_name: " << file;
      return -1;
    }

    google::protobuf::io::FileInputStream pb_file_fd(file_fd);
    pb_file_fd.SetCloseOnDelete(true);
    if (!google::protobuf::TextFormat::Parse(&pb_file_fd, obj)) {
      LOG(ERROR) << "parse file pb content failed. file_name: " << file;
      return -1;
    }
    return 0;
  }

  template<typename PbType>
  static int ParsePbFromString(const std::string& buffer, PbType* obj) {
    if (!google::protobuf::TextFormat::ParseFromString(buffer, obj)) {
      LOG(ERROR) << "parse pb from string failed. buffer: " << buffer;
      return -1;
    }
    return 0;
  }
}; // class ProtobufOp

} // namespace
#endif // OPENMI_BASE_PROTOBUF_OP_H_
