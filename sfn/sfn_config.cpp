#include "framework.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>

#include "sfn.h"

const char def_config[] =
    "space space\n"
    "j left\n"
    "k down\n"
    "l right\n"
    "i up\n"
    "u home\n"
    "o end\n"
    "h pageup\n"
    "n pagedown\n"
    "us_semicomma del\n"
    "us_quote back\n"
    "m us_tilde\n";

struct vknm_map_t {
    DWORD vk;
    const char* name;
};

const vknm_map_t vknm_map[] = {
    {VK_BACK, "back"},
    {VK_RETURN, "ret"},
    {VK_ESCAPE, "esc"},
    {VK_SPACE, "space"},
    {VK_PRIOR, "pageup"},
    {VK_NEXT, "pagedown"},
    {VK_HOME, "home"},
    {VK_END, "end"},
    {VK_LEFT, "left"},
    {VK_RIGHT, "right"},
    {VK_UP, "up"},
    {VK_DOWN, "down"},
    {VK_INSERT, "ins"},
    {VK_DELETE, "del"},
    {VK_OEM_PLUS, "plus"},
    {VK_OEM_MINUS, "minus"},
    {VK_OEM_COMMA, "comma"},
    {VK_OEM_PERIOD, "period"},
    //below are for US layout
    {VK_OEM_1, "us_semicomma"},
    {VK_OEM_2, "us_slash"},
    {VK_OEM_3, "us_tilde"},
    {VK_OEM_4, "us_leftbr"},
    {VK_OEM_5, "us_backslash"},
    {VK_OEM_6, "us_rightbr"},
    {VK_OEM_7, "us_quote"},
    {0, NULL}
};

inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline bool iequals(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

static DWORD nm2vk(std::string name)
{
    rtrim(name);
    ltrim(name);
    if (name.empty())
        return 0;
    if (name.length() == 1) {
        char ch = name[0];
        if ((ch >= '0' && ch <= '9')
            || (ch >= 'A' && ch <= 'Z')) {
            return (DWORD)ch;
        }
        if (ch >= 'a' && ch <= 'z') {
            return (DWORD)(ch - 'a' + 'A');
        }
        return 0;
    }
    const vknm_map_t* p = vknm_map;
    while (p->name) {
        if (iequals(name, p->name))
            return p->vk;
        p++;
    }
    return 0;
}

void load_config()
{
    std::istream* config = new std::ifstream("sfn.cfg");
    if (config->fail()) {
        delete config;
        config = new std::istringstream(def_config);
    }

    memset(g_kmap, 0, sizeof(g_kmap));
    g_act_vk = VK_SPACE;

    std::string line;
    std::ostringstream msg("following errors in config file", std::ios_base::ate);
    int i = 0, err = 0;
    while (std::getline(*config, line)) {
        i++;
        rtrim(line);
        ltrim(line);
        if (line.empty())
            continue;
        std::string::size_type pos = line.find(' ');
        DWORD k1 = nm2vk(line.substr(0, pos));
        if (k1 == 0) {
            msg << std::endl << "    line " << i << "  token: " << line.substr(0, pos);
            err = 1;
            continue;
        }

        DWORD k2 = nm2vk(line.substr(pos + 1, std::string::npos));
        if (k2 == 0) {
            msg << std::endl << "    line " << i << "  token: " << line.substr(pos + 1, std::string::npos);
            err = 1;
            continue;
        }
        if (k1 == k2) {
            g_act_vk = k1;
        }
        g_kmap[k1].vkt = k2;
        g_kmap[k1].scode = MapVirtualKey(k2, 0);
    }
    delete config;
    if (err > 0) {
        MessageBoxA(NULL, msg.rdbuf()->str().c_str(), "error", MB_OK);
    }
}
