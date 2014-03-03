# Place all the behaviors and hooks related to the matching controller here.
# All this logic will automatically be available in application.js.
# You can use CoffeeScript in this file: http://coffeescript.org/

$ ->

  canvas_container = $('#canvas_container')
  canvas_element = canvas_container[0]
  @paper = new Raphael(canvas_element, 750, 750)
  reset_button = $('#reset-button')
  window.splatter = this

  reset_button.click (e) =>
    @paper.clear()


  @tmp_line

  @drag_function = (m) =>
    if @tmp_line
      @tmp_line.remove()
    @end = Point.new_from_click_event(m)
    @tmp_line = @paper.path(PathStrings.line(@start, @end))
    console.log "poop"

  canvas_container.mousedown (e) =>
    window.foobar = e
    @start = Point.new_from_click_event(e)
    canvas_container.mousemove @drag_function

  canvas_container.mouseup (e) =>
    @splatter()
    if @tmp_line
      @tmp_line.remove()
    canvas_container.unbind("mousemove", @drag_function)

  @splatter = =>
    return unless @tmp_line

    end_radius = 50
    start_radius = 20

    @paper.circle(@start.x, @start.y, 20).attr("fill", "#f00").attr("stroke", "#f00")
    @paper.circle(@end.x, @end.y, 50).attr("fill", "#f00").attr("stroke", "#f00")

    start_circle_tip_1 = RaphMath.perpendicular_point(@start, @end, start_radius)
    start_circle_tip_2 = RaphMath.perpendicular_point(@start, @end, -start_radius)
    end_circle_tip_1 = RaphMath.perpendicular_point(@end, @start, end_radius)
    end_circle_tip_2 = RaphMath.perpendicular_point(@end, @start, -end_radius)

    pd_1 = RaphMath.perpendicular_bisector(@start, @end, -20).p
    pd_2 = RaphMath.perpendicular_bisector(@start, @end, 20).p

    console.log pd_1
    console.log pd_2

    p_string = PathStrings.s_curve(start_circle_tip_1, end_circle_tip_2, pd_1) + PathStrings.line_to(end_circle_tip_1) + PathStrings.s_curve_to(start_circle_tip_2, pd_2) + PathStrings.line_to(start_circle_tip_1)
    pth = @paper.path(p_string).attr("fill", "#f00").attr("stroke", "#f00")



  @first = new Point(50, 250)
  @second = new Point(200, 500)
