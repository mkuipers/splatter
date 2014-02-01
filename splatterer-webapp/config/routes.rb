SplattererWebapp::Application.routes.draw do
  resources :splatter, only: [:index]
end
