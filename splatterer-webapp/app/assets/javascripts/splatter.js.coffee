# Place all the behaviors and hooks related to the matching controller here.
# All this logic will automatically be available in application.js.
# You can use CoffeeScript in this file: http://coffeescript.org/

$ ->

  canvas_container = $('#canvas_container')
  canvas_element = canvas_container[0]
  @paper = new Raphael(canvas_element, 750, 750)
  reset_button = $('#reset-button')
  window.foo = this

  @state = 'ready'

  reset_button.click (e) =>
    @paper.clear()


  # Document states
  #  ready
  #  drawing

  # canvas_container.click (e) ->
  #   #if @state === 'ready'


  @first = new Point(50, 250)
  @second = new Point(200, 500)





