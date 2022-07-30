from conans import ConanFile

class PackageConan(ConanFile):
    requires = ["libcurl/7.84.0", "tidy-html5/5.8.0", "pugixml/1.12.1", "libxml2/2.9.14"]
    generators = "cmake"