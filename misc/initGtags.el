;; work in progress: init emacs to use gtags
;; http://www.wolinlabs.com/blog/emacs.global.speedbar.html

(setq load-path (cons "/opt/local/share/gtags/" load-path))

(autoload 'gtags-mode "gtags" "" t)

(setq gtags-rootdir "/Users/bgschaid/OpenFOAM/Subversion/swak4Foam_dev")

(when window-system       ;start speedbar if we're using a window system like X, etc
   (speedbar t))

(which-function-mode 1)

(add-hook 'c++-mode-hook
   '(lambda ()
      (gtags-mode t)
))
