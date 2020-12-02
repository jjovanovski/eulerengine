import sys
import os

args = sys.argv

if len(args) == 1:
    print("Type --help for help")
    exit()

cmd = args[1]

if cmd == "-h" or cmd == "--help":
    print("Available commands:")
    print(" => addclass <class_name> <path>")
    exit()

if cmd == "addclass":
    if len(args) != 4:
        print(" => addclass <class_name> <path>")
        exit()
    
    class_name = args[2]
    path = args[3]

    h_file = path + "/" + class_name + ".h"
    cpp_file = path + "/" + class_name + ".cpp"

    h_file_template = """#pragma once

namespace Euler
{
    namespace Vulkan
    {
        class EULER_API {ClassName}
        {
        private:

        public:
        
        };
    };
};
"""

    cpp_file_template = """#include "{ClassName}.h"

using namespace Euler::Vulkan;
"""

    # create h file
    f = open(h_file, "w")
    f.write(h_file_template.replace("{ClassName}", class_name))
    f.close()

    # create cpp file
    f = open(cpp_file, "w")
    f.write(cpp_file_template.replace("{ClassName}", class_name))
    f.close()

    # maybe run cmake here?
    #os.system("premake5 vs2019")
