import datetime

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

index = clang.cindex.Index.create()
translation_unit = index.parse(args[1], args=["-x", "c++", "-std=c++17"])


def filter_node_list_by_node_kind(
        nodes: typing.Iterable[clang.cindex.Cursor],
        kinds: list
) -> typing.Iterable[clang.cindex.Cursor]:
    result = []

    for i in nodes:
        if i.kind in kinds:
            result.append(i)

    return result


def find_all_exposed_fields(
        cursor: clang.cindex.Cursor
):
    result = []

    field_declarations = filter_node_list_by_node_kind(cursor.get_children(), [clang.cindex.CursorKind.FIELD_DECL])

    for i in field_declarations:
        if i.access_specifier != clang.cindex.AccessSpecifier.PUBLIC:
            continue

        result.append(i)

    return result


all_classes = filter_node_list_by_node_kind(translation_unit.cursor.get_children(),
                                            [clang.cindex.CursorKind.CLASS_DECL])


def generate_class_member_array(
        class_cursor: clang.cindex.Cursor
):
    result = []

    fields = find_all_exposed_fields(class_cursor)

    for i in fields:
        field_type = i.type.spelling
        field_name = i.spelling

        result.append(
            f"{{ ComputeCrc32(\"{field_name}\", VG_ARRAY_SIZE(\"{field_name}\") - 1), \"{field_name}\", ComputeCrc32(\"{field_type}\", VG_ARRAY_SIZE(\"{field_type}\") - 1), \"{field_type}\",0,sizeof({field_type}) }},\n") \

    return f"const ClassMember k{class_cursor.spelling}ClassMembers[] = {{\n" + "".join(result) + "};"


def generate_class_reflection(class_node):
    inplace_members = generate_class_member_array(class_node)
    override = f"VG_REFLECTED_IMPL({class_node.spelling})"

    return f"// +Reflection(\"{class_node.spelling}\")\n" + inplace_members + "\n" + override + "\n" + f"// -Reflection(\"{class_node.spelling}\")\n"


def find_offset_to_reflection(class_node, file):
    # Find // +Reflection("ClassName")
    # Find // -Reflection("ClassName")

    with open(file, "r") as f:
        lines = f.readlines()

    start_line = -1
    end_line = -1

    for i in range(len(lines)):
        print(lines[i])
        if "+Reflection(\"" + class_node.spelling + "\")" in lines[i]:
            start_line = i
        if "-Reflection(\"" + class_node.spelling + "\")" in lines[i]:
            end_line = i

    if start_line == -1 or end_line == -1:
        return -1, -1

    return start_line, end_line


for i in all_classes:
    target_file = i.location.file.name
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
        print(f"Skipping class \"{i.spelling}\", no reflection found")
        continue

    with open(target_file, "w") as f:
        f.writelines(lines)
