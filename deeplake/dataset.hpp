#include <filesystem>

using namespace std;

namespace deeplake {

    class dataset {
    public:
        explicit dataset(const string &path);
        explicit dataset(const filesystem::path &path);

        const string &path() const;

    private:
        string path_;
    };

    dataset create(const string &path);

}
