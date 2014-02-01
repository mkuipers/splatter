
class Point
  constructor: (x, y) ->
    @x = x
    @y = y

  to_string: ->
    "#{@x},#{@y}"

#Expects p's as { x: <number>, y: <number> }
#Returns point as { x: <number>, y: <number> }
@midpoint = (p1, p2) ->
  x = (p1.x + p2.x) / 2
  y = (p1.y + p2.y) / 2
  return new Point(x, y)

#Expects p's as { x: <number>, y: <number> }
#Expects length as <number>
#Returns two points as { p: <point>, p2: <point> }
@perpendicular_bisector = (p1, p2, length) ->
  angle = Math.atan2(p2.y - p1.y, p2.x - p1.x)
  v2 = @midpoint(p1, p2)
  v1 = { x: length * Math.sin(angle) + v2.x, y: -length * Math.cos(angle) + v2.y }
  return { p: new Point(v1.x, v1.y), mid: new Point(v2.x, v2.y) }

rad = Math.PI / 180;




@draw_s_curve = (p1, p2, pd, paper) ->
  pathstring = "M#{p1.to_string()} S#{pd.to_string()}, #{p2.to_string()}"
  console.log pathstring
  paper.path("M"+p1.to_string()+"S"+ pd.to_string() + "," + p2.to_string())
#@draw_qb_curve = (p1, p2, pd)

@draw_line = (p1, p2) ->
  pathstring = "M#{p1.to_string()} L#{p2.to_string()} "
  @paper.path(pathstring)

