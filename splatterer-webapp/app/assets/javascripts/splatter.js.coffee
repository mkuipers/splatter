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
    if @tmp_line
      @tmp_line.remove()
    canvas_container.unbind("mousemove", @drag_function)

  @first = new Point(50, 250)
  @second = new Point(200, 500)





