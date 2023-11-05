
# VigilSDK

C++ Runtime Reflection, And General-Purpose Development Tool Kit. 

![Logo](https://i.ibb.co/NtNPcHz/capybara-pfp.png)


## Usage

**Class Reflection**, in this example it shows a sample Object with Foo and Bar member variables. 

**Note:** When you want to use class reflection, it's important to add the +/- Reflection markers so that the code parser knows where to insert the class info.
***
```cpp
#include <Common/ObjectModel/Object.h>

class TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    vgBool Foo;
    vgU32 Bar;

};

// +Reflection(ClassName:TestObject)
// -Reflection(ClassName:TestObject)
```

Run the `Reflect.py` script.
```
usage: Vigil Class Generator [-h] [-file file [file ...]] [-v [cpp_version]] [-llvm_path [llvm_path]] [-save_tmp [save_tmp]]

Generates reflection code for classes

options:
  -h, --help            show this help message and exit
  -file file [file ...]
                        file to parse
  -v [cpp_version], --cpp_version [cpp_version]
                        C++ version to use
  -llvm_path [llvm_path]
                        Path to libclang
  -save_tmp [save_tmp]  Save temporary file after parsing
```

To add reflection to your class, you can run the following command:
```
py .\Reflect.py -file <file> -llvm_path <path to libclang>
```

And that's it! You should now be ready to use the reflection api and all it has to offer.
## License

[MIT](https://choosealicense.com/licenses/mit/)
