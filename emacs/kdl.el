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



;variables
(setq woman-use-own-frame nil); show manuals in the same frame.

(global-set-key "\M-`" 'hippie-expand) ; Come up with autocompletions; surprizingly useful.
(global-set-key "\C-x u" 'undo) ; Mor convinient for undo
(global-set-key "\C-c t" 'toggle-truncate-lines) ; Wrap-unwrap lines

(setq truncate-partial-width-windows nil) ; Enable wrapping-inwrapping for split buffers.

;; ~/.emacs_$SHELLNAME - srartup code for shells in emacs.
