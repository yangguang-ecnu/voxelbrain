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

(if (x-list-fonts "6x10")
    (set-default-font "6x10"))

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
(global-set-key "\C-cm" 'transient-mark-mode) ; Transient mark for local undo (COOL)

(setq truncate-partial-width-windows nil) ; Enable wrapping-inwrapping for split buffers.

;;cygwin setup

;; tweaks for cygwin; can be enabled manually, some trickery for 
;; automatic enableing provided.

(defun cygwin()
  "Make default shell to be bash"
  (interactive)
  (setq shell-file-name "bash")
  (setq shell-command-switch "-c")      ; SHOULD IT BE (setq shell-command-switch "-ic")?
  (setq explicit-shell-file-name "bash")
  (setenv "SHELL" explicit-shell-file-name)
  (setq w32-quote-process-args ?\"))

(if
    (or
     (file-exists-p "c:/bin/bash.exe")
     (file-exists-p "c:/cygwin/bin/bash.exe"))
    (progn
      (cygwin)))


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
