type Tree:
  Node { ~left , ~right }
  Leaf { val }

def reduction_sum(tree):
  fold tree:
    case Tree/Node:
      return tree.left + tree.right
    case Tree/Leaf:
      return tree.val

def treefy(first, last):
  bend f = first, l = last:
    when l > f:
      mid = (f + l) / 2
      tree = Tree/Node { left: fork(f,mid) , right: fork(mid+1,l) }
    else:
      tree = Tree/Leaf { val: f }
  return tree

def main:
  tree = treefy(1,9)
  return reduction_sum(tree)

