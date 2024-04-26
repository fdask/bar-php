<?php
class Door {
    private $orig;
    private $dest;
    private $dir;
    private $origX;
    private $destX;

    public static $dirUp = "UP";
    public static $dirDown = "DOWN";

    public function __construct($orig, $origX, $dest, $destX, $dir) {
        $this->orig = $orig;
        $this->origX = $origX;
        $this->dest = $dest;
        $this->destX = $destX;
        $this->dir = $dir;
    }

    public function getDir() {
        return $this->dir;
    }

    public function getOrig() {
        return $this->orig;
    }

    public function getOrigX() {
        return $this->origX;
    }

    public function getDest() {
        return $this->dest;
    }

    public function getDestX() {
        return $this->destX;
    }

    public function getX() {
        return $this->x;
    }
}