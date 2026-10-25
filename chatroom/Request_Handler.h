//
// Created by 李玉辉 on 2026/10/25.
//

#ifndef RASPI_REQUEST_HANDLER_H
#define RASPI_REQUEST_HANDLER_H
#include <string>


class Request_Handler {
public:
    std::string handle(const std::string& request);

private:
    struct CpuTimes
    {
        std::uint64_t idle;
        std::uint64_t total;
    };

    CpuTimes previous_cpu_{};
    bool has_previous_cpu_ = false;

    CpuTimes read_cpu_times();
    std::string get_cpu_usage();
    std::string get_hostname();
    std::string get_system_info();
};



#endif //RASPI_REQUEST_HANDLER_H
