Write in Pseudo Code what the code generation (calling to the backend) of templated structs would be like (from the AST)


Here's a pseudo-code representation of how code generation for templated structs might work when processing the AST:

```python
def generate_templated_struct(ast_node):
    """
    Process a templated struct AST node and generate corresponding backend code
    """

    # Store all generated template specializations
    template_specializations = {}

    def process_template_struct(struct_node, template_params):
        # Generate unique name for this template specialization
        specialized_name = generate_specialized_name(struct_node.name, template_params)

        # Check if we've already generated this specialization
        if specialized_name in template_specializations:
            return template_specializations[specialized_name]

        # Create new struct type for this specialization
        struct_type = create_struct_type(specialized_name)

        # Process template parameters
        param_mappings = {}
        for param, concrete_type in zip(struct_node.template_params, template_params):
            param_mappings[param.name] = concrete_type

        # Process each field in the struct
        for field in struct_node.fields:
            field_type = substitute_template_params(field.type, param_mappings)

            # Handle nested template instantiations if needed
            if is_template_type(field_type):
                field_type = generate_templated_struct(
                    field_type.template_node,
                    field_type.concrete_params
                )

            add_field_to_struct(struct_type, field.name, field_type)

        # Generate methods
        for method in struct_node.methods:
            specialized_method = specialize_method(
                method,
                param_mappings,
                struct_type
            )
            add_method_to_struct(struct_type, specialized_method)

        # Store generated specialization
        template_specializations[specialized_name] = struct_type
        return struct_type

    def process_template_instantiation(instantiation_node):
        template_struct = instantiation_node.template_ref
        concrete_params = instantiation_node.params

        # Validate template parameters
        validate_template_params(template_struct, concrete_params)

        # Generate specialized version
        return process_template_struct(template_struct, concrete_params)

    # Main generation logic based on node type
    if is_template_definition(ast_node):
        # Just register the template definition for later use
        register_template_definition(ast_node)
        return None
    elif is_template_instantiation(ast_node):
        return process_template_instantiation(ast_node)
    else:
        raise Error("Invalid AST node for template processing")

def substitute_template_params(type_expr, param_mappings):
    """
    Replace template parameters in a type expression with concrete types
    """
    if is_template_param(type_expr):
        return param_mappings[type_expr.name]
    elif is_compound_type(type_expr):
        # Recursively substitute in compound types (arrays, etc)
        return substitute_in_compound_type(type_expr, param_mappings)
    else:
        return type_expr

def generate_specialized_name(base_name, template_params):
    """
    Generate unique name for template specialization
    Example: Vector<int> -> Vector_int
    """
    param_names = [param.name for param in template_params]
    return f"{base_name}_{'_'.join(param_names)}"

def specialize_method(method_node, param_mappings, struct_type):
    """
    Create specialized version of a method with concrete types
    """
    # Create new method with substituted parameter and return types
    specialized_method = new_method_node()
    specialized_method.name = method_node.name
    specialized_method.return_type = substitute_template_params(
        method_node.return_type,
        param_mappings
    )

    # Process parameters
    for param in method_node.parameters:
        specialized_param = substitute_template_params(param.type, param_mappings)
        add_parameter(specialized_method, param.name, specialized_param)

    # Specialize method body
    specialized_method.body = specialize_method_body(
        method_node.body,
        param_mappings,
        struct_type
    )

    return specialized_method
```

This pseudo-code outlines the main components needed for generating code from templated struct AST nodes:

1. **Main Template Processing**:
   - Handles both template definitions and instantiations
   - Maintains a cache of generated specializations
   - Creates unique specialized versions for each combination of template parameters

2. **Type Substitution**:
   - Replaces template parameters with concrete types
   - Handles nested template instantiations
   - Processes compound types recursively

3. **Method Specialization**:
   - Creates specialized versions of methods with concrete types
   - Substitutes types in parameters and return values
   - Processes method bodies with concrete types

4. **Name Generation**:
   - Creates unique names for specialized types
   - Ensures no naming conflicts between different specializations

Some important considerations not fully shown in the pseudo-code:

- **Dependency Tracking**: Need to track dependencies between template instantiations
- **Circular References**: Need to handle potential circular template references
- **Error Handling**: Validate template parameters and report meaningful errors
- **Optimization**: Might want to defer actual code generation until needed
- **Debug Info**: Need to maintain mapping between generated code and source

The actual implementation would need to:
1. Interface with the specific backend code generation system
2. Handle language-specific details and requirements
3. Implement proper symbol resolution and scope handling
4. Generate appropriate debug information
5. Handle platform-specific alignment and padding rules

Could you modify the code so that it handles circular templates ?
So that for instance, the following code is correct:
```
struct <T> tree => {
  public val: T;
  public children: vector<tree<T>*>
}
```

Here's the modified pseudo-code that handles circular template references:

```python
class TemplateGenerator:
    def __init__(self):
        # Cache of completed specializations
        self.template_specializations = {}
        # Track in-progress specializations to handle circular refs
        self.in_progress_specializations = set()
        # Forward declarations for circular references
        self.forward_declarations = {}

    def generate_templated_struct(self, ast_node):
        """
        Process a templated struct AST node and generate corresponding backend code
        """
        def process_template_struct(struct_node, template_params):
            # Generate unique name for this template specialization
            specialized_name = generate_specialized_name(struct_node.name, template_params)

            # Check if we've already generated this specialization
            if specialized_name in self.template_specializations:
                return self.template_specializations[specialized_name]

            # Check for circular reference
            if specialized_name in self.in_progress_specializations:
                # Return or create forward declaration
                if specialized_name not in self.forward_declarations:
                    self.forward_declarations[specialized_name] = create_forward_declaration(specialized_name)
                return self.forward_declarations[specialized_name]

            # Mark this specialization as in-progress
            self.in_progress_specializations.add(specialized_name)

            # Create new struct type for this specialization
            struct_type = create_struct_type(specialized_name)

            # Create forward declaration if needed
            if specialized_name not in self.forward_declarations:
                self.forward_declarations[specialized_name] = create_forward_declaration(specialized_name)

            # Process template parameters
            param_mappings = {}
            for param, concrete_type in zip(struct_node.template_params, template_params):
                param_mappings[param.name] = concrete_type

            # First pass: declare all fields (types only)
            for field in struct_node.fields:
                field_type = substitute_template_params(field.type, param_mappings)

                # Handle nested template instantiations
                if is_template_type(field_type):
                    # For circular references, use pointer indirection
                    if is_self_referential(field_type, specialized_name):
                        field_type = create_pointer_type(self.forward_declarations[specialized_name])
                    else:
                        field_type = generate_templated_struct(
                            field_type.template_node,
                            field_type.concrete_params
                        )

                declare_field(struct_type, field.name, field_type)

            # Second pass: complete the struct definition
            complete_struct_definition(struct_type)

            # Generate methods
            for method in struct_node.methods:
                specialized_method = specialize_method(
                    method,
                    param_mappings,
                    struct_type
                )
                add_method_to_struct(struct_type, specialized_method)

            # Store generated specialization and remove from in-progress
            self.template_specializations[specialized_name] = struct_type
            self.in_progress_specializations.remove(specialized_name)

            return struct_type

        def is_self_referential(field_type, current_specialized_name):
            """Check if a field type refers to the current template specialization"""
            if not is_template_type(field_type):
                return False

            target_name = generate_specialized_name(
                field_type.template_node.name,
                field_type.concrete_params
            )
            return target_name == current_specialized_name

        def create_forward_declaration(specialized_name):
            """Create a forward declaration for a struct"""
            forward_decl = create_incomplete_struct_type(specialized_name)
            return forward_decl

        # Main generation logic
        if is_template_definition(ast_node):
            register_template_definition(ast_node)
            return None
        elif is_template_instantiation(ast_node):
            return process_template_instantiation(ast_node)
        else:
            raise Error("Invalid AST node for template processing")

    def process_template_instantiation(self, instantiation_node):
        template_struct = instantiation_node.template_ref
        concrete_params = instantiation_node.params

        # Validate template parameters
        validate_template_params(template_struct, concrete_params)

        # Generate specialized version
        return self.process_template_struct(template_struct, concrete_params)

# Example usage for the tree structure:
"""
struct <T> tree => {
    public val: T;
    public children: vector<tree<T>*>
}
"""

def process_tree_example():
    # Create AST nodes (simplified)
    tree_template = TemplateStructNode(
        name="tree",
        template_params=[TemplateParam("T")],
        fields=[
            Field("val", TemplateParamType("T")),
            Field("children",
                  TemplateType(
                      "vector",
                      [PointerType(
                          TemplateType("tree", [TemplateParamType("T")])
                      )]
                  ))
        ]
    )

    # Create generator
    generator = TemplateGenerator()

    # Generate tree<int> specialization
    int_tree = generator.generate_templated_struct(
        TemplateInstantiation(tree_template, [IntType()])
    )

    return int_tree
```

Key changes to handle circular templates:

1. **Forward Declarations**:
   - Added tracking of forward declarations
   - Creates incomplete type declarations when needed
   - Uses forward declarations to break circular dependencies

2. **Two-Pass Generation**:
   - First pass declares the structure and fields
   - Second pass completes the definition
   - Allows circular references to work properly

3. **In-Progress Tracking**:
   - Tracks templates currently being generated
   - Detects circular references during generation
   - Uses forward declarations when circular references are detected

4. **Pointer Indirection**:
   - Automatically handles self-referential structures through pointers
   - Prevents infinite recursion in type generation

5. **Specialization Cache**:
   - Maintains cache of both complete and in-progress specializations
   - Prevents duplicate generation and infinite loops

For the tree example:
- When generating `tree<T>`, it first creates a forward declaration
- The `children` field uses a pointer to `tree<T>`, breaking the circular reference
- The vector container can be properly instantiated with the pointer type
- The complete structure is generated after all fields are processed

This approach allows for complex recursive template structures while ensuring:
1. No infinite recursion during generation
2. Proper handling of circular references
3. Correct type relationships in the generated code
4. Efficient generation with caching
5. Clean handling of pointer indirection where needed
