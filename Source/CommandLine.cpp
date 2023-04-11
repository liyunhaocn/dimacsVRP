
#include "CommandLine.h"

namespace hust{
    void to_json(Json& j, CommandLine& cl) {
        j = Json{
                {"seed", cl.seed},
                {"runTimer", cl.runTimer},
                {"dimacsPrint", cl.dimacsPrint},
                {"infoFlag", cl.infoFlag},
                {"debugFlag", cl.debugFlag},
                {"errorFlag", cl.errorFlag}
        };
        Json t;
        to_json(t, cl.aps);
        j["aps"] = t;
    }

    void from_json(const Json& j, CommandLine& cl) {

        j.at("seed").get_to(cl.seed);
        j.at("runTimer").get_to(cl.runTimer);
        j.at("dimacsPrint").get_to(cl.dimacsPrint);
        j.at("infoFlag").get_to(cl.infoFlag);
        j.at("debugFlag").get_to(cl.debugFlag);
        j.at("errorFlag").get_to(cl.errorFlag);

        Logger::infoFlag = cl.infoFlag;
        Logger::debugFlag = cl.debugFlag;
        Logger::errorFlag = cl.errorFlag;

        auto& apsJson = j.at("AlgorithmParameters");
        from_json(apsJson, cl.aps);
    }
}
