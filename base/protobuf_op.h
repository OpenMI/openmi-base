/*!
 *  Copyright (c) 2016 by Contributors
 *  \file protobuf_op.h
 *  \brief protobuf op. io/convert/...
 */
#ifndef OPENMI_BASE_PROTOBUF_OP_H_
#define OPENMI_BASE_PROTOBUF_OP_H_ 

#include <fcntl.h>   // open
#include <memory>
#include <fstream>

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
  /*!
   * \brief parse from text file
   */
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

  /*!
   * \brief parse from text string format.
   */
  template<typename PbType>
  static int ParsePbFromString(const std::string& buffer, PbType* obj) {
    if (!google::protobuf::TextFormat::ParseFromString(buffer, obj)) {
      LOG(ERROR) << "parse pb from string failed. buffer: " << buffer;
      return -1;
    }
    return 0;
  }

  /*!
   * \brief serialize to string. Notice: not text format
   */
  template <typename PbType>
  static int SerializeToString(PbType* obj, std::string& serialized_string) {
    if (! obj->SerializeToString(&serialized_string)) {
      LOG(ERROR) << "serialize to string from protobuf obj failed. pb:" << obj->DebugString();
      return -1;
    }
    return 0;
  }

  /*!
   * \brief parse from serailzied string
   */
  template <typename PbType>
  static int ParseFromString(const std::string& serialized_buffer, PbType* obj) {
    if (! obj->ParseFromString(serailized_buffer)) {
      LOG(ERROR) << "parse from serialized string failed."; 
      return -1;
    }
    return 0;
  }

  template <typename PbType>
  static int SerializeToOstream(const char* file_path, PbType* obj) {
    fstream ofs(file_path, ios::out|ios::trunc|ios::binary);
    bool result = obj->SerailizeToOstream(&ofs);
    ofs.close();
    ofs.clear();
    if (result) {
      LOG(ERROR) << "serailize to ostream failed. file:" 
        << file_path << ", pb:" << obj->DebugString();
      return -1;
    }
    return 0;
  }

  template <typename PbType>
  static int ParseFromIstream(const char* file_path, PbType* obj) {
    fstream ifs(file_path, ios::in|ios::binary);
    bool result = obj->ParseFromIstream(&ifs);
    ifs.close();
    ifs.clear();
    if (!result) {
      LOG(INFO) << "parse from istream failed. file:" << file_path;
      return -1;
    }
    return 0;
  }

}; // class ProtobufOp

} // namespace
#endif // OPENMI_BASE_PROTOBUF_OP_H_
