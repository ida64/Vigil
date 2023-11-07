# (C) 2023 Zel Software, SP
# Please review the license provided before using this project in any capacity.
#
# This script is used to generate reflection code for classes.
# Please use --help for more information.

import os
import sys
import logging
import re

import argparse
from pprint import pprint

import clang.cindex
import typing

logging.basicConfig(format='%(asctime)s %(levelname)s: %(message)s', datefmt='%H:%M:%S', level=logging.INFO)

parser = argparse.ArgumentParser(
    prog='Vigil Class Generator',
    description='Generates reflection code for classes')

parser.add_argument('-file', metavar='file', type=str, nargs='+', help='file to parse')
parser.add_argument('-src_dir', metavar='src', type=str, nargs='?', help='Source/ directory of VigilSDK')
parser.add_argument('-v', '--cpp_version', metavar='cpp_version', type=str, nargs='?', default='c++20',
                    help='C++ version to use')
parser.add_argument('-llvm_path', metavar='llvm_path', type=str, nargs='?', help='Path to libclang')
parser.add_argument('-save_tmp', metavar='save_tmp', type=bool, nargs='?', default=False,
                    help='Save temporary file after parsing')

args = parser.parse_args()

if args.file is None:
    logging.error("No file specified, view help with --help")
    sys.exit(1)

if args.llvm_path is None:
    logging.error("No llvm path specified, view help with --help")
    sys.exit(1)

if args.src_dir is None:
    logging.error("No source directory specified, view help with --help")
    sys.exit(1)

config = clang.cindex.Config()
config.set_library_file(args.llvm_path)

sourceFile = args.file[0]

logging.info(f"Analyzing \"{sourceFile}\"")

# Copy to temp file
tmpFile = sourceFile + ".tmp"
with open(sourceFile, "r") as f:
    lines = f.readlines()
    with open(tmpFile, "w") as f2:
        f2.writelines(lines)

logging.info(f"Using temporary file \"{tmpFile}\"")

replaceList = [
    ("vgBool", "bool"),
    ("vgChar", "char"),
    ("vgU8", "uint8_t"),
    ("vgS8", "int8_t"),
    ("vgU16", "uint16_t"),
    ("vgS16", "int16_t"),
    ("vgU32", "uint32_t"),
    ("vgS32", "int32_t"),
    ("vgU64", "uint64_t"),
    ("vgS64", "int64_t"),
    ("vgFloat", "float"),
    ("VG_CLASS_REFLECTION", "__attribute__((annotate(\"VG_CLASS_REFLECTION\")))"),
]

class ReflectedObject:
    def __init__(self, options):
        self.options = options

    def serialize(self):
        options_str = ', '.join(f'{name}:{value}' for name, value in self.options.items())
        return f'Reflection({options_str})'

    def get_option(self, name, default=None):
        return self.options.get(name, default)

    def set_option(self, name, value):
        self.options[name] = value

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

# replace in temp file
with open(tmpFile, "r") as f:
    lines = f.readlines()
    for i in range(len(lines)):
        for j in replaceList:
            lines[i] = lines[i].replace(j[0], j[1])
    with open(tmpFile, "w") as f2:
        f2.writelines(lines)

index = clang.cindex.Index.create()

# get parent directory
translation_unit = index.parse(tmpFile, args=["-x", "c++", "-std=" + args.cpp_version,
                                              "-I" + args.src_dir])
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
def generate_class_member_array(class_cursor: clang.cindex.Cursor):
    result = []
    fields = find_all_exposed_fields(class_cursor)

    for field in fields:
        field_type = field.type.spelling
        field_name = field.spelling
        field_size = f"sizeof({field_type})"
        reflection_flags = ReflectedObject.deserialize(field.raw_comment)

        if field.type.kind == clang.cindex.TypeKind.CONSTANTARRAY:
            reflection_flags.set_option("Flags", "Flags_ConstantArray")
            field_type = field.type.element_type.spelling
            logging.info(f"Found constant array \"{field_name}\" of type \"{field_type}\" (Line {field.location.line})")

        if field.type.kind == clang.cindex.TypeKind.POINTER:
            if is_class_reflected(field.type.get_pointee().get_declaration()):
                print(f"{field_name} is a pointer to a reflected class")
                reflection_flags.set_option("Flags", "Flags_Pointer")

                # remove the * from the type name
                field_type = field.type.get_pointee().get_declaration().spelling


        flags = (reflection_flags.get_option("Flags", "Flags_None").replace("Flags_", "ClassMember::Flags_"))
        logging.info(f"Found field \"{field_name}\" of type \"{field_type}\" (Line {field.location.line})"
                     f" with flags \"{flags}\"")

        result.append(
            f"    {{ VG_CRC32(\"{field_name}\"),"  # m_ID
            f"\"{field_name}\","  # m_Name
            f"VG_CRC32(\"{field_type}\"),"  # m_TypeID
            f"\"{field_type}\","  # m_Type
            f"offsetof({class_cursor.spelling}, {field_name}),"  # m_Offset
            f"{field_size},"  # m_Size
            f"{flags} }},\n")  # m_Flags

    return f"const vigil::ClassMember k{class_cursor.spelling}ClassMembers[] = {{\n" + "".join(result) + "};"


def find_all_exposed_enums(cursor: clang.cindex.Cursor):
    result = []

    enum_declarations = filter_node_list_by_node_kind(cursor.get_children(), [clang.cindex.CursorKind.ENUM_DECL])

    for enum_declaration in enum_declarations:
        if enum_declaration.access_specifier != clang.cindex.AccessSpecifier.PUBLIC:
            continue

        result.append(enum_declaration)

    return result

# Generate class member array in the form of:
# const ClassEnum k<ClassName>Enums[] = {
#  { "EnumName", VG_CRC32("EnumName"), 2, {
#   { "EnumValue1", VG_CRC32("EnumValue1"), 0 },
# ...
def generate_enums_array(class_cursor: clang.cindex.Cursor):
    result = []
    enums = find_all_exposed_enums(class_cursor)
    value_arrays = []
    enum_names = []

    for enum in enums:
        enum_name = enum.spelling
        logging.info(f"Found enum \"{enum_name}\" (Line {enum.location.line})")

        for enum_value in enum.get_children():
            if enum_value.kind != clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
                continue

            value_name = enum_value.spelling
            logging.info(f"Found enum value \"{value_name}\" (Line {enum_value.location.line})")

            value_arrays.append(
                f"    {{ \"{value_name}\", VG_CRC32(\"{value_name}\"), {enum_value.enum_value} }}")

        result.append(
            f"const vigil::EnumValue k{class_cursor.spelling}{enum_name}Values[] ="
            f" {{\n" + ",\n".join(value_arrays) + "\n};\n")

        result.append(
            f"const vigil::ClassEnum k{class_cursor.spelling}{enum_name} = {{ \"{enum_name}\","
            f" VG_CRC32(\"{enum_name}\"),"
            f"k{class_cursor.spelling}{enum_name}Values,"
            f" VG_ARRAY_SIZE(k{class_cursor.spelling}{enum_name}Values) }};")

        value_arrays.clear()
        enum_names.append(f"k{class_cursor.spelling}{enum_name}")

    return ("".join(result), enum_names)


def generate_class_reflection(class_node):
    # get default constructor
    default_constructor = None
    for constructor in filter_node_list_by_node_kind(class_node.get_children(),
                                                        [clang.cindex.CursorKind.CONSTRUCTOR]):
            if constructor.is_default_constructor():
                default_constructor = constructor
                break

    if default_constructor is None:
        logging.warning(f"Could not find default constructor for class \"{class_node.spelling}\"")
        return "// ERROR: A default constructor is required for reflection, i.e. \"ClassName() = default;\""

    logging.info(f"Found default constructor for class \"{class_node.spelling}\" (Line {default_constructor.location.line})")

    inplace_members = generate_class_member_array(class_node)

    reflected_object = ReflectedObject({"ClassName": class_node.spelling})
    reflected_object_str = reflected_object.serialize()

    enums_arr = generate_enums_array(class_node)
    override = f"VG_REFLECTED_IMPL({class_node.spelling});"

    return (f"// +{reflected_object_str}\n" + inplace_members
            + "\n" + "".join(enums_arr[0])
            + f"\nconst vigil::FixedArray<const ClassEnum*, {len(enums_arr[1])}> k{class_node.spelling}Enums = {{ {', '.join('&' + x for x in enums_arr[1])} }};"
            + f"\n{override}\n" + f"// -{reflected_object_str}\n")


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


def is_class_reflected(class_node):
    attributes = class_node.get_children()
    for attribute in attributes:
        if attribute.spelling == "VG_CLASS_REFLECTION":
            return True

for class_node in filter_node_list_by_node_kind(translation_unit.cursor.get_children(),
                                                [clang.cindex.CursorKind.CLASS_DECL]):
    if not is_class_reflected(class_node):
        continue

    target_file = sourceFile
    if target_file == "":
        continue

    with open(target_file, "r") as f:
        lines = f.readlines()

    offset = find_offset_to_reflection(class_node, target_file)
    if offset[0] != -1 and offset[1] != -1:
        logging.info(f"Found class \"{class_node.spelling}\", replacing reflection between {offset[0]} and {offset[1]}")

        # remove old lines
        del lines[offset[0]:offset[1] + 1]
        # insert new lines
        lines.insert(offset[0], generate_class_reflection(class_node))

        logging.info(f"{class_node.spelling} OK")
    else:
        logging.warning(f"Could not find reflection for class \"{class_node.spelling}\"")
        continue

    with open(target_file, "w") as f:
        f.writelines(lines)

if not args.save_tmp:
    os.remove(tmpFile)
