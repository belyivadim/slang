import sys
import io

def define_boilerplate_methods(header_file: io.TextIOWrapper, class_name: str) -> None:
    indent = "  "
    header_file.write(indent + class_name + f"(const {class_name}&) = default;\n")
    header_file.write(indent + class_name + f"({class_name}&&) = default;\n")
    header_file.write(indent + class_name + "& " + f"operator=(const {class_name}&) = default;\n")
    header_file.write(indent + class_name + "& " + f"operator=({class_name}&&) = default;\n")


def define_value_getter(header_file: io.TextIOWrapper) -> None:
    header_file.write("template <class Visitor, class Visitable, class R>\n")
    header_file.write("class ValueGetter {\n")
    header_file.write("public:\n")
    header_file.write("  ValueGetter() = default;\n")
    define_boilerplate_methods(header_file, "ValueGetter")
    header_file.write("  virtual ~ValueGetter() = default;\n\n")
    # header_file.write("  static R GetValue(Visitable& visitable) {\n")
    # header_file.write("    Visitor visitor;\n")
    # header_file.write("    visitable.accept(visitor);\n")

    header_file.write("  R& GetValue(Visitable& visitable) {\n")
    header_file.write("    visitable.accept(*(static_cast<Visitor*>(this)));\n")
    header_file.write("    return m_value;\n")
    header_file.write("  }\n\n")

    header_file.write("  void Return(const R& value) {\n")
    header_file.write("    m_value = value;\n")
    header_file.write("  }\n\n")
    header_file.write("private:\n")
    header_file.write("  R m_value;\n\n")
    header_file.write("};\n\n")


def define_visitor(header_file: io.TextIOWrapper, base_name: str, types: list[str]) -> None:
    header_file.write("class IVisitor {\n")

    header_file.write("public:\n")
    header_file.write("  IVisitor() = default;\n")
    define_boilerplate_methods(header_file, "IVisitor")
    header_file.write("  virtual ~IVisitor() = default;\n\n")

    for t in types:
        type_name = t.split("with")[0].strip()
        header_file.write("  virtual void visit" + type_name + base_name + "(" +
                          type_name + "& " + base_name.lower() + ") = 0;\n")

    header_file.write("};\n\n")


def define_type(header_file: io.TextIOWrapper, base_name: str, class_name: str, fields: str) -> None:
    header_file.write("class " + class_name + " : public " + base_name + " {\n")
    header_file.write("public:\n")

    fields_splited = fields.split(", ")

    # ctor
    header_file.write("  " + class_name + "(" + fields + ") :\n")
    header_file.write(f"    {base_name}(),\n")
    for i in range(fields_splited.__len__()):
        field = fields_splited[i]
        name = field.split(" ")[1]

        end = "," if i != fields_splited.__len__() - 1 else "\n  {}\n"
        header_file.write(f"    m_{name}({name}){end}\n")

    define_boilerplate_methods(header_file, class_name)
    header_file.write("  virtual ~" + class_name + "() = default;\n\n")

    header_file.write("  void accept(IVisitor& visitor) override {\n")
    header_file.write(f"    visitor.visit{class_name}{base_name}(*this);\n")
    header_file.write("  }\n\n")

    # fields
    for field in fields_splited:
        field_type, name = field.split(" ")
        header_file.write(f"  {field_type} m_{name};\n")

    header_file.write("\n};\n\n")


def define_ast(output_dir: str, base_name: str, 
               includes_std: list[str], includes_user: list[str],
               types: list[str]) -> None:
    path = output_dir + "/" + base_name + ".hpp"

    with open(path, "w", encoding="UTF-8") as header_file:
        header_file.write(f"#ifndef __SLANG_{base_name.upper()}_HPP__\n")
        header_file.write(f"#define __SLANG_{base_name.upper()}_HPP__\n\n")

        for include in includes_std:
            header_file.write(f"#include <{include}>\n")
        header_file.write("\n")

        for include in includes_user:
            header_file.write(f"#include \"{include}\"\n")
        header_file.write("\n")


        header_file.write("namespace slang {\n\n")

        header_file.write("namespace " + base_name.lower() + " {\n\n")

        # forward decls
        for t in types:
            class_name = t.split("with")[0].strip()
            header_file.write(f"class {class_name};\n")

        header_file.write("\n")

        # visitor interface
        define_visitor(header_file, base_name, types)

        # base class
        header_file.write("class " + base_name + " {\npublic:\n")
        header_file.write(f"  {base_name}() = default;\n")
        define_boilerplate_methods(header_file, base_name)
        header_file.write("  virtual ~" + base_name + "() = default;\n\n")
        header_file.write("  virtual void accept(IVisitor& visitor) = 0;\n\n")
        header_file.write("};\n\n")

        # ValueGetter
        define_value_getter(header_file)

        # derived classes
        for type in types:
            splited = type.split("with")
            class_name, fields = splited[0].strip(), splited[1].strip()
            define_type(header_file, base_name, class_name, fields)


        header_file.write("} // namespace expr\n\n")
        header_file.write("} // namespace slang\n\n")
        header_file.write(f"#endif // __SLANG_{base_name.upper()}_HPP__\n")


def main() -> None:
    if sys.argv.__len__() != 2:
        print("Usage: python3 ast_class_generator.py <output directory>")
        exit(64)

    output_dir = sys.argv[1]
    define_ast(output_dir, "Expr",
        ["memory"],
        ["Token.hpp"],
        [
        "Assign     with Token name, std::shared_ptr<Expr> value",
        "Binary     with std::shared_ptr<Expr> left, Token oper, std::shared_ptr<Expr> right",
        "Grouping   with std::shared_ptr<Expr> expression",
        "Literal    with Object value",
        "Unary      with Token oper, std::shared_ptr<Expr> right",
        "Variable   with Token name"
        ])

    define_ast(output_dir, "Stmt", 
        ["memory"],
        ["Token.hpp", "Expr.hpp"],
        [
        "Expression with std::shared_ptr<expr::Expr> expression",
        "Print      with std::shared_ptr<expr::Expr> expression",
        "Var        with Token name, std::shared_ptr<expr::Expr> initializer"
        ])


if __name__ == "__main__":
    main()
