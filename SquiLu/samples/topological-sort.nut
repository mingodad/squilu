auto recipes = [
    ["martian stew", "thingy,pig,cheese danish,chicken"],
    ["cheese danish", "flour,butter,egg,vanilla,cream cheese,sugar"],
    ["butter", "milk,salt"],
    ["thingy", "iron,apple,vanilla"],
    ["cream cheese", "milk,salt"],
    ["chicken", "worm"],
    ["worm", "apple"],
    ["egg", "chicken"],
    ["milk", "cow"],
    ["cow", "grass"],
    ["pig", "apple,worm"]
];

auto function to_graph(recipes)
{
    //get recipes into a canonical form for further manipulation
    auto nodes = [];
    auto nodemap = {};
    local function add_node(name)
    {
	auto node;
        if( name in nodemap )
            node = nodemap[name];
        else
	{
            node = {"name": name};
            nodes.append(node);
            nodemap[name] <- node;
	}
        return node;
    }
    foreach( recipe, ingredients in recipes )
    {
        ingredients = ingredients.split(",");
        auto node = add_node(recipe);
        node["edges"] <- ingredients;
        foreach( ingredient in ingredients )
            node = add_node(ingredient);
    }
    foreach( node in nodes )
    {
        if( "edges" in node )
            node["edges"] = tuple(nodemap[ingredient] for ingredient in node["edges"]);
        else
            node["edges"] = ();
    }
    return nodes;
}

// Print a list of the nodes along with the outgoing edges of each:
foreach( n in to_graph(recipes) )
{
    print(format("%-20s", n["name"]), [n2["name"] for n2 in n["edges"]]);
}