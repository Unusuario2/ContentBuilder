$modelname "brick/brick001_wall/brick001_wall_a"

$cdmaterials "models/brick001_wall/"

$surfaceprop "brick"

$staticprop

$origin 0.000000 0.000000 0.000000 -90.000000

$scale 1.000000

$body "brick001_wall_a" "brick001_wall_a.SMD"

$collisionmodel "brick001_wall_collision.SMD" {
    $concave
    $maxconvexpieces 10000
}

$sequence "idle" "anims/idle.SMD"
