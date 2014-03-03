
class Point
  @new_from_click_event = (e) ->
    new Point(e.offsetX, e.offsetY)

  constructor: (x, y) ->
    @x = x
    @y = y

  to_string: ->
    "#{@x},#{@y}"


class RaphMath
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

  # p_interest is a point to draw the perpendicular from
  # p_other is any other point on the line you want to draw perpendicular to
  # length is how far youwant along that perpendicular you want to go
  @perpendicular_point = (p_interest, p_other, length) ->
    angle = Math.atan2(p_other.y - p_interest.y, p_other.x - p_interest.x)
    new Point(length * Math.sin(angle) + p_interest.x, -length * Math.cos(angle) + p_interest.y)


class PathStrings

  ## ~ Curves ~
  #String for moving the turtle to this space
  @move_to = (p) ->
    "M#{p.to_string()} "
  #String for drawing S curve to a location
  @s_curve_to = (p_end, pd) ->
    "S#{pd.to_string()}, #{p_end.to_string()} "
  #String for moving and drawing an s curve from that location
  @s_curve = (p_start, p_end, pd) ->
    @move_to(p_start) + @s_curve_to(p_end, pd)

  ## ~ Lines ~
  @line_to = (p) ->
    "L#{p.to_string()} "
  #String representing drawing a string between two points
  @line = (p_start, p_end) ->
    @move_to(p_start) + @line_to(p_end)




  # @draw_s_curve = (p1, p2, pd, paper) ->
  #   pathstring = "M#{p1.to_string()} S#{pd.to_string()}, #{p2.to_string()}"
  #   console.log pathstring
  #   paper.path("M"+p1.to_string()+"S"+ pd.to_string() + "," + p2.to_string())
  # #@draw_qb_curve = (p1, p2, pd)

  # @draw_line = (p1, p2) ->
  #   pathstring = "M#{p1.to_string()} L#{p2.to_string()} "
  #   @paper.path(pathstring)



$ ->
  window.Point = Point
  window.PathStrings = PathStrings
  window.RaphMath = RaphMath