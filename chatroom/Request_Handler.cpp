//
// Created by 李玉辉 on 2026/10/25.
//

#include "Request_Handler.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string>


using json = nlohmann::json;
// 请求分发
std::string Request_Handler::handle(const std::string& request)
{
    try
    {
        json req = json::parse(request);

        std::string type = req.at("type").get<std::string>();

        json response = {
            {"type", type},
            {"request_id", req.value("request_id", 0)},
            {"success", true}
        };

        if (type == "GET_HOSTNAME")
        {
            response["data"] = {
                {"hostname", get_hostname()}
            };
        }
        else if (type == "GET_SYSTEM_INFO")
        {
            response["data"] = get_system_info();
        }
        else if (type == "GET_CPU_USAGE")
        {
            response["data"] = {
                {"cpu_usage", get_cpu_usage()}
            };
        }
        else
        {
            response["success"] = false;
            response["error"] = "UNKNOWN_COMMAND";
        }

        return response.dump();
    }
    catch (const std::exception& e)
    {
        return json{
                {"success", false},
                {"error", e.what()}
        }.dump();
    }
}

// 获取主机名
std::string Request_Handler::get_hostname()
{
    char hostname[256] = {};

    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        return "ERROR: gethostname failed";
    }

    hostname[sizeof(hostname) - 1] = '\0';
    return std::string(hostname);
}

// 获取系统信息
json Request_Handler::get_system_info()
{
    struct sysinfo info{};

    if (sysinfo(&info) != 0)
    {
        throw std::runtime_error("sysinfo failed");
    }

    const auto total_memory =
        static_cast<unsigned long long>(info.totalram) * info.mem_unit;

    const auto free_memory =
        static_cast<unsigned long long>(info.freeram) * info.mem_unit;

    return {
            {"uptime", info.uptime},
            {"memory_total_mb", total_memory / 1024 / 1024},
            {"memory_free_mb", free_memory / 1024 / 1024}
    };
}

#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

Request_Handler::CpuTimes Request_Handler::read_cpu_times()
{
    std::ifstream file("/proc/stat");

    if (!file)
        throw std::runtime_error("Cannot open /proc/stat");

    std::string line;
    std::getline(file, line);

    std::istringstream iss(line);

    std::string cpu;
    std::uint64_t user, nice, system, idle;
    std::uint64_t iowait, irq, softirq, steal;

    if (!(iss >> cpu >> user >> nice >> system >> idle
              >> iowait >> irq >> softirq >> steal)
        || cpu != "cpu")
    {
        throw std::runtime_error("Invalid /proc/stat");
    }

    CpuTimes result;

    // iowait 也计入空闲时间（近似处理）
    result.idle = idle + iowait;

    result.total = user + nice + system + idle
                 + iowait + irq + softirq + steal;

    return result;
}

json Request_Handler::get_cpu_usage()
{
    CpuTimes current = read_cpu_times();

    if (!has_previous_cpu_)
    {
        previous_cpu_ = current;
        has_previous_cpu_ = true;

        return nullptr;
    }

    const auto total_delta =
        current.total - previous_cpu_.total;

    const auto idle_delta =
        current.idle - previous_cpu_.idle;

    previous_cpu_ = current;

    if (total_delta == 0)
    {
        return nullptr;
    }

    double usage = 100.0 *
        (1.0 - static_cast<double>(idle_delta) / total_delta);

    return usage;
}