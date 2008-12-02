;emacs setting for Konstantin Leivnski

;Add the following to .emacs:
;(setq load-path 
;      (cons "~/.emacs.d" load-path))
;(load-library "kdl")

;loading libraries

;; Subversion
(load-library "vc-svn")
(load-library "psvn")
(add-to-list 'vc-handled-backends 'SVN)

(tool-bar-mode 0) ; Disable toolbars and menubars for real estate.
(menu-bar-mode 0)
(set-default-font "6x10");; TODO - check that the font is avaliable.

;;Interactive buffers, from http://www.emacswiki.org/emacs-en/InteractivelyDoThings#toc6.
(require 'ido)
(ido-mode t)

;;Winner mode
(when (fboundp 'winner-mode)
      (winner-mode 1))

;variables
(setq woman-use-own-frame nil); show manuals in the same frame.

(global-set-key "\M-`" 'hippie-expand) ; Come up with autocompletions; surprizingly useful.
(global-set-key "\C-ct" 'toggle-truncate-lines) ; Wrap-unwrap lines
(global-set-key "\C-cg" 'find-file-at-point) ; Go to whatever is under cursor
(global-set-key "\C-ck" 'kill-buffer) ; Alternative binding for killing buffer

(setq truncate-partial-width-windows nil) ; Enable wrapping-inwrapping for split buffers.

(global-set-key

;; function for customizing
(defun kdl () 
  "open kdl.el"
  (interactive)
  (find-file "~/.emacs.d/kdl.el"))

;;scratch stuff
(defun kdlt () 
  "Kdl Test"
  (interactive)
  (let
      ((url (thing-at-point 'url))
       (file (thing-at-point 'filename)))
    (progn 
      (if (file-exists-p file)
	  (print "file") (print "url")))))

;;FYI
;; ~/.emacs_$SHELLNAME - startup code for shells in emacs.
