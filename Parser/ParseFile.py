import datetime
from pprint import pprint

import clang.cindex
import typing

import sys

args = sys.argv

if len(args) < 2:
    print("Usage: ParseFile.py <file>")
    sys.exit(1)

# get config
config = clang.cindex.Config()
config.set_library_file("C:/Program Files/LLVM/bin/libclang.dll")

sourceFile = args[1]

print("Parsing file: " + sourceFile)

# Copy to temp file
tmpFile = sourceFile + ".tmp"
with open(sourceFile, "r") as f:
    lines = f.readlines()
    with open(tmpFile, "w") as f2:
        f2.writelines(lines)

print("Temp file: " + tmpFile)

replaceList = [
    ("vgBool", "bool"),
    ("vgU8", "uint8_t"),
    ("vgS8", "int8_t"),
    ("vgU16", "uint16_t"),
    ("vgS16", "int16_t"),
    ("vgU32", "uint32_t"),
    ("vgS32", "int32_t"),
    ("vgU64", "uint64_t"),
    ("vgS64", "int64_t"),
    ("vgString", "const char*"),
]

# replace in temp file
with open(tmpFile, "r") as f:
    lines = f.readlines()
    for i in range(len(lines)):
        for j in replaceList:
            lines[i] = lines[i].replace(j[0], j[1])
            print(f"{tmpFile}:{lines[i]}:{j[0]} => {j[1]}")
    with open(tmpFile, "w") as f2:
        f2.writelines(lines)

index = clang.cindex.Index.create()
translation_unit = index.parse(tmpFile, args=["-x", "c++", "-std=c++17"])


def filter_node_list_by_node_kind(nodes: typing.Iterable[clang.cindex.Cursor], kinds: list) -> typing.Iterable[
    clang.cindex.Cursor]:
    result = []

    for node in nodes:
        if node.kind in kinds:
            result.append(node)

    return result


def find_all_exposed_fields(cursor: clang.cindex.Cursor):
    result = []

    field_declarations = filter_node_list_by_node_kind(cursor.get_children(), [clang.cindex.CursorKind.FIELD_DECL])

    for field_declaration in field_declarations:
        if field_declaration.access_specifier != clang.cindex.AccessSpecifier.PUBLIC:
            continue

        result.append(field_declaration)

    return result


# Generate class member array in the form of:
# const ClassMember k<ClassName>ClassMembers[] = {...
def generate_class_member_array(
        class_cursor: clang.cindex.Cursor
):
    result = []

    # Find all exposed (public) fields in the class
    fields = find_all_exposed_fields(class_cursor)

    for field in fields:
        field_type = field.type.spelling
        field_name = field.spelling

        # Inject flags into the result from the comment
        flags = (ReflectedObject.deserialize(field.raw_comment)
                 .get_option("Flags", "Flags_None")
                 .replace("Flags_", "ClassMember::Flags_"))

        # Append the result
        result.append(
            f"{{ VG_CRC32(\"{field_name}\"),"  # m_ID
            f"\"{field_name}\","  # m_Name
            f"VG_CRC32(\"{field_type}\"),"  # m_TypeID
            f"\"{field_type}\","  # m_Type
            f"offsetof({class_cursor.spelling}, {field_name}),"  # m_Offset
            f"sizeof({class_cursor.spelling}),"  # m_Size
            f"{flags} }},\n")  # m_Flags

    return f"const ClassMember k{class_cursor.spelling}ClassMembers[] = {{\n" + "".join(result) + "};"


class ReflectedObject:
    def __init__(self, options):
        self.options = options

    def serialize(self):
        options_str = ', '.join(f'{name}:{value}' for name, value in self.options.items())
        return f'Reflection({options_str})'

    def get_option(self, name, default=None):
        return self.options.get(name, default)

    @classmethod
    def deserialize(cls, s):
        if s is None:
            return cls({})

        start = s.find("(") + 1
        end = s.find(")", start)
        if start == 0 or end == -1:
            return None
        options_str = s[start:end]
        options = dict(option.split(":") for option in options_str.split(", "))
        return cls(options)


def generate_class_reflection(class_node):
    inplace_members = generate_class_member_array(class_node)
    override = f"VG_REFLECTED_IMPL({class_node.spelling})"

    reflected_object = ReflectedObject({"ClassName": class_node.spelling})
    reflected_object_str = reflected_object.serialize()

    return f"// +{reflected_object_str}\n" + inplace_members + "\n" + override + "\n" + f"// -{reflected_object_str}\n"


def find_offset_to_reflection(class_node, file):
    with open(file, "r") as f:
        lines = f.readlines()

    start_line = -1
    end_line = -1

    for i in range(len(lines)):
        if "+Reflection" in lines[i] and class_node.spelling in lines[i]:
            start_line = i

        if "-Reflection" in lines[i] and class_node.spelling in lines[i]:
            end_line = i

    if start_line == -1 or end_line == -1:
        return -1, -1

    return start_line, end_line


for class_node in filter_node_list_by_node_kind(translation_unit.cursor.get_children(),
                                                [clang.cindex.CursorKind.CLASS_DECL]):
    target_file = sourceFile
    if target_file == "":
        continue

    with open(target_file, "r") as f:
        lines = f.readlines()

    offset = find_offset_to_reflection(i, target_file)
    if offset[0] != -1 and offset[1] != -1:
        # remove old lines
        del lines[offset[0]:offset[1] + 1]
        # insert new lines
        lines.insert(offset[0], generate_class_reflection(i))
    else:
        print(f"Skipping class \"{class_node.spelling}\", no reflection found")
        continue

    with open(target_file, "w") as f:
        f.writelines(lines)

import os

os.remove(tmpFile)
