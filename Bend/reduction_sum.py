def reduction_sum(tree):
    if tree['is_leaf'] is True:
        return tree['val']
    return reduction_sum(tree['left']) + reduction_sum(tree['right'])

def treefy(first, last):
    node = {}
    if last > first:
        mid = (last + first) // 2
        node['left'] = treefy(first, mid)
        node['right'] = treefy(mid+1, last)
        node['is_leaf'] = False
    else:
        node['val'] = first
        node['is_leaf'] = True

    return node

def main():
    tree = treefy(1,9)
    print(reduction_sum(tree))

main()
