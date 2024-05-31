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
  bend i = 0, inc = 2:
    when i < 2000:
      tree = treefy(1,inc)
      sum = List/Cons { head: reduction_sum(tree) , tail: fork(i+1,inc+1) }
    else:
      sum = List/Nil
  return sum

