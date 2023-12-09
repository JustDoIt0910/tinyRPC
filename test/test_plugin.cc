//
// Created by just do it on 2023/12/7.
//
#include "google/protobuf/compiler/cpp/cpp_generator.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/printer.h"
#include "../plugin/proto/method_ext.pb.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

class MyGenerator : public google::protobuf::compiler::cpp::CppGenerator {
    bool Generate(const FileDescriptor* file, const std::string& parameter,
                  GeneratorContext* generator_context,
                  std::string* error) const override {
        auto stream = generator_context->Open("plg_out.txt");
        Printer printer(stream, ' ');

        printer.PrintRaw("input params: \n");
        std::vector<std::pair<std::string, std::string>> params;
        ParseGeneratorParameter(parameter, &params);
        for(const auto& p: params) {
            printer.PrintRaw(p.first + "=" + p.second + "\n");
        }
        printer.PrintRaw("\n");

        printer.PrintRaw("messages: \n");
        for(int i = 0; i < file->message_type_count(); i++) {
            auto message = file->message_type(i);
            printer.PrintRaw(message->name() + "\n");
        }
        printer.PrintRaw("\n");

        auto service = file->service(0);
        printer.PrintRaw("service: \n");
        printer.PrintRaw(service->DebugString());
        printer.PrintRaw("methods in this service: \n");
        for(int i = 0; i < service->method_count(); i++) {
            auto method = service->method(i);
            auto options = method->options();
            auto r = options.GetExtension(tinyRPC::api::http);

            printer.PrintRaw(method->name() + "\n");
            auto input = method->input_type();
            auto output = method->output_type();
            printer.PrintRaw("method input: " + input->name() + "\n");
            printer.PrintRaw("method output: " + output->name() + "\n");

            printer.PrintRaw("option: \n");
            printer.PrintRaw(r.url() + "\n");
            printer.PrintRaw(r.method()+ "\n");
        }

        return true;
    }
};

int main(int argc, char* argv[]) {
    MyGenerator generator;
    return PluginMain(argc, argv, &generator);
}