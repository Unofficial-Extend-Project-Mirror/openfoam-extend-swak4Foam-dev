;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil .
      ((projectile-project-compilation-cmd . "./Allwmake")
       (lsp-auto-guess-root . t)))
 (c++-mode .
           ((eval . (if (fboundp 'lsp) (lsp) nil)))))
